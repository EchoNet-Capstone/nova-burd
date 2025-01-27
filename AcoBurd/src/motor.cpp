#include <Arduino.h>

#include "motor.h"
#include "my_clock.h"
#include "get_set_macros.h"
#include "timers.h"
#include "device_state.h"
#include "watchdog.h"

GET_SET_FUNC_DEF(int, motor_target, 0);
GET_SET_FUNC_DEF(bool, is_motor_running, false);

void motor_init(void){
  // Setup motor quadrature and interrupt
  pinMode(MOTOR_QUAD_A, INPUT);
  pinMode(MOTOR_QUAD_B, INPUT);
  attachInterrupt(MOTOR_QUAD_A, motor_quadrature_interrupt, RISING);

  // Setup motor driver - Uses 1-2 mA
  pinMode(MOTOR_DRIVER_POWER, OUTPUT);
  pinMode(MOTOR_DRIVER_A, OUTPUT);
  pinMode(MOTOR_DRIVER_B, OUTPUT);
  digitalWrite(MOTOR_DRIVER_POWER, LOW);
  digitalWrite(MOTOR_DRIVER_A, LOW);
  digitalWrite(MOTOR_DRIVER_B, LOW);

  // ROTATE SHAFT ON BOOT!!!
  motor_run_to_position(CLOSED_POSITION);
  delay(5000);
  motor_run_to_position(OPEN_POSITION);
}

// Wiggle the motor to clear barnacles
void check_wiggle(){
  // Wiggle motor to break barnacles if release is closed
  if((get_wiggle_timer() >= InternalClock()) || (get_release_is_open() == false)) {
    return;
  }

  // If we're more than wiggle_deadband seconds from opening the release, then allow a wiggle
  if(get_wiggle_timer() >= (get_release_timer() - WIGGLE_DEADBAND)){
    return;
  }

  set_wiggle_timer(InternalClock() + WIGGLE_INTERVAL);

  int motor_temporary_position = get_motor_position();

  motor_run_to_position(motor_temporary_position + 250);
  delay(1000);
  // Pet the watchdog
  feedInnerWdt();
  motor_run_to_position(motor_temporary_position - 250);
  delay(1300);
  // Pet the watchdog
  feedInnerWdt();
  motor_run_to_position(motor_temporary_position);
  delay(1000);
  // Pet the watchdog
  feedInnerWdt();

  // Need to reset this here because it gets called on boot - this causes this subroutine to run twice
  set_wiggle_timer(InternalClock() + WIGGLE_INTERVAL);
}

void motor_service(void){
  check_wiggle();

  // Power down encoder and motor driver if it's been a while since motor has been energized
  if(get_encoder_timer() < InternalClock()){
    motor_sleep();
  }
}

void set_motor_state(){
  if(get_release_timer() > InternalClock()){
    motor_run_to_position(CLOSED_POSITION);
    // If there is time on the clock, not waiting to be recovered
    set_waiting_to_be_retrieved(false);
  }
  else{
    motor_run_to_position(OPEN_POSITION);
  }
}

// Motor Power Save
void motor_sleep(){
  digitalWrite(MOTOR_DRIVER_POWER, LOW);
}

// Motor Wake Up
void motor_wake_up(){
  set_encoder_timer(InternalClock() + ENCODER_TIMEOUT);
  digitalWrite(MOTOR_DRIVER_POWER, HIGH);
}

// Turn Motor Off
void motor_off(){
  digitalWrite(MOTOR_DRIVER_A, LOW);
  digitalWrite(MOTOR_DRIVER_B, LOW);
}

// Run Motor Reverse
void motor_reverse(){
  motor_wake_up();
  digitalWrite(MOTOR_DRIVER_A, HIGH);
  digitalWrite(MOTOR_DRIVER_B, LOW);
}

// Run Motor Forward
void motor_forward(){
  motor_wake_up();
  digitalWrite(MOTOR_DRIVER_A, HIGH);
  digitalWrite(MOTOR_DRIVER_B, HIGH);
}

// Motor encoder interrupt
void motor_quadrature_interrupt(){
  if(digitalRead(MOTOR_QUAD_B)){
    set_motor_position(get_motor_position() + 1);
  }
  else set_motor_position(get_motor_position() - 1);

  if(abs(get_motor_position() - get_motor_target()) < MOTOR_DEADBAND){
    motor_off();
    set_is_motor_running(false);
  }
  else if(motor_target < get_motor_position()){
    motor_forward();
    set_is_motor_running(true);
  }
  else if(motor_target > get_motor_position()){
    motor_reverse();
    set_is_motor_running(true);
  }

  // Set encoder time to one second in the future if motor is running
  if(get_is_motor_running() == true){
    //encoder_timer = InternalClock() + ENCODER_TIMEOUT;
  }
}

// Set motor target position
void motor_run_to_position(int target){
  motor_target = target;

  // Give the motor an initial kick so the interrupt routine can take over
  if(abs(motor_target - get_motor_position()) < MOTOR_DEADBAND){
    motor_off();
  }
  else if(motor_target < get_motor_position()){
    // Activate display

    set_display_timer(InternalClock() + DISPLAY_TIMEOUT);
    motor_wake_up();
    motor_forward();
  }
  else if(motor_target > get_motor_position()){
    // Activate display

    set_display_timer(InternalClock() + DISPLAY_TIMEOUT);
    motor_wake_up();
    motor_reverse();
  }
}