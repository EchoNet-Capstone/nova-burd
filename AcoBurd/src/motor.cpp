#include "motor.h"
#include "globals.h"
#include "my_clock.h"
#include "subroutines.h"
#include "watchdog.h"
#include "Arduino.h"

int motor_position;
int motor_target;
int motor_target_last;
bool last_quad_a_state;
bool last_quad_b_state;
bool is_motor_running;
int motor_last_position1;
int motor_last_position2;
int motor_last_position3;
long wiggle_timer;

void set_motor_position(int new_motor_position){
  motor_position = new_motor_position;
}

int get_motor_position(void){
  return motor_position;
}
void set_motor_target(int new_motor_target){
  motor_target = new_motor_target;
}

int get_motor_target(void){
  return motor_target;
}
void set_motor_target_last(int new_motor_target_last){
  motor_target_last = new_motor_target_last;
}

int get_motor_target_last(void){
  return motor_target_last;
}
void set_last_quad_a_state(bool new_last_quad_a_state){
  last_quad_a_state = new_last_quad_a_state;
}

bool get_last_quad_a_state(void){
  return last_quad_a_state;
}
void set_last_quad_b_state(bool new_last_quad_b_state){
  last_quad_b_state = new_last_quad_b_state;
}

bool get_last_quad_b_state(void){
  return last_quad_b_state;
}
void set_is_motor_running(bool new_is_motor_running){
  is_motor_running = new_is_motor_running;
}

bool get_is_motor_running(void){
  return is_motor_running;
}
void set_motor_last_position1(int new_motor_last_position1){
  motor_last_position1 = new_motor_last_position1;
}

int get_motor_last_position1(void){
  return motor_last_position1;
}
void set_motor_last_position2(int new_motor_last_position2){
  motor_last_position2 = new_motor_last_position2;
}

int get_motor_last_position2(void){
  return motor_last_position2;
}
void set_motor_last_position3(int new_motor_last_position3){
  motor_last_position3 = new_motor_last_position3;
}

int get_motor_last_position3(void){
  return motor_last_position3;
}

void set_wiggle_timer(long new_wiggle_timer){
  wiggle_timer = new_wiggle_timer; 
}

long get_wiggle_timer(){
  return wiggle_timer;
}

// Motor Power Save
void motor_sleep(){
  digitalWrite(motor_driver_power, LOW);
}

// Motor Wake Up
void motor_wake_up(){
  set_encoder_timer(InternalClock() + encoder_timeout);
  digitalWrite(motor_driver_power, HIGH);
}

// Turn Motor Off
void motor_off(){
  digitalWrite(motor_driver_a, LOW);
  digitalWrite(motor_driver_b, LOW);
}

// Run Motor Reverse
void motor_reverse(){
  motor_wake_up();
  digitalWrite(motor_driver_a, HIGH);
  digitalWrite(motor_driver_b, LOW);
}

// Run Motor Forward
void motor_forward(){
  motor_wake_up();
  digitalWrite(motor_driver_a, HIGH);
  digitalWrite(motor_driver_b, HIGH);
}

// Motor encoder interrupt
void motor_quadrature_interrupt(){
  if(digitalRead(motor_quad_b)){
    motor_position++;
  }
  else motor_position--;

  if(abs(motor_position - motor_target) < motor_deadband){
    motor_off();
    is_motor_running = 0;
  }
  else if(motor_target < motor_position){
    motor_forward();
    is_motor_running = 1;
  }
  else if(motor_target > motor_position){
    motor_reverse();
    is_motor_running = 1;
  }

  // Set encoder time to one second in the future if motor is running
  if(is_motor_running == 1){
    //encoder_timer = InternalClock() + encoder_timeout;
  }
}

// Set motor target position
void motor_run_to_position(int target){
  motor_target = target;

  // Give the motor an initial kick so the interrupt routine can take over
  //Serial.printf("Motor currently at %d.  Run to position %d.\n", motor_position, motor_target);
  if(abs(motor_target - motor_position) < motor_deadband){
    motor_off();
    //if ( !is_motor_running && ( motor_position == motor_last_position1 ) && ( motor_last_position1 == motor_last_position2 ) && ( motor_last_position2 == motor_last_position3 ) ) motor_sleep();    // Shut down motor encoder and driver
  }
  else if(motor_target < motor_position){
    set_display_timer(InternalClock() + display_timeout);                                                // Activate display
    motor_wake_up();
    motor_forward();
  }
  else if(motor_target > motor_position){
    set_display_timer(InternalClock() + display_timeout);                                                // Activate display
    motor_wake_up();
    motor_reverse();
  }

  //motor_last_position3 = motor_last_position2;
  //motor_last_position2 = motor_last_position1;
  //motor_last_position1 = motor_position;

  //motor_target_last = motor_target;
}

// Wiggle the motor to clear barnacles
void wiggle_motor(){
  int motor_temporary_position = motor_position;
  
  motor_run_to_position(motor_temporary_position + 250);
  delay(1000);
  feedInnerWdt();           // Pet the watchdog
  motor_run_to_position(motor_temporary_position - 250);
  delay(1300);
  feedInnerWdt();           // Pet the watchdog
  motor_run_to_position(motor_temporary_position);
  delay(1000);
  feedInnerWdt();           // Pet the watchdog

  set_wiggle_timer(InternalClock() + wiggle_interval);                       // Need to reset this here because it gets called on boot - this causes this subroutine to run twice
}