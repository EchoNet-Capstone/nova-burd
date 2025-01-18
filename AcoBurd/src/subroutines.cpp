#include "stdio.h"
#include "subroutines.hpp"
#include "motor.hpp"
#include "globals.hpp"
#include "watchdog.hpp"
#include "sleep.hpp"
#include <Arduino.h>

// timer vars
 long display_timer;
 long encoder_timer;
 long release_timer;
 long release_timer1;
 long release_timer2;
 long battery_timer;
 long time_until_release;

// battery
 int battery_percent = 0;
 bool sleep_inhibit = 1;                        // Default to staying awake on boot
 bool display_active = 0;
 bool reed_switch1 = 0;
 bool reed_switch2 = 0;
 bool last_reed_switch_state = 0;
 bool waiting_to_be_retrieved = 0;
 bool release_is_open = 0;
 bool release_last_position = 0;
 bool is_led_activated = 0;
 bool gps_enabled = 0;
 bool gps_lock = 0;
 bool input_slowdown_toggle = 0;

void VextON() {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

long get_encoder_timer(){
  return encoder_timer;
}

void set_encoder_timer(long new_encoder){
  encoder_timer = new_encoder;
}

void VextOFF() {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}


void set_motor_state() {
  if (get_release_timer() > InternalClock() ) {
    motor_run_to_position(closed_position);
    waiting_to_be_retrieved = 0;                                                        // If there is time on the clock, not waiting to be recovered
  }
  else {
    motor_run_to_position(open_position);
  }
}

long get_release_timer(){
  return release_timer;
}

void set_release_timer(long new_release_timer){
  release_timer = new_release_timer;
}

long get_battery_timer(){
  return battery_timer;
}

void set_battery_timer(long new_battery_timer){
   battery_timer = new_battery_timer;
}

long get_time_until_release(){
  return time_until_release;
}

void set_time_until_release(long new_time_until_release){
  time_until_release = new_time_until_release;
}

long get_display_timer(){
  return display_timer;
}

void set_display_timer(long new_display){
  display_timer = new_display;
}

// Measure battery voltage
uint16_t sampleBatteryVoltage() {
  noInterrupts();
  VextON();

  uint16_t volts = getBatteryVoltage();

  interrupts();                                                                         // Reenable interrupts after sample

  uint16_t battery_usable_volts = battery_full - battery_empty;

  battery_percent = (int)( (100 * (volts - battery_empty) ) / battery_usable_volts);
  if (battery_percent > 100){
    battery_percent = 100;
  }
  if (battery_percent < 0){
    battery_percent = 0;
  }

  if (battery_percent < low_battery){
    set_release_timer(InternalClock());
  }             // Release trap if battery gets low

  return volts;
}

void reed_switch_debounce() {
  long time_delta;

  reed_switch1 = !digitalRead(reed_switch_input1);                                                                        // Signal is inverted
  reed_switch2 = !digitalRead(reed_switch_input2);                                                                        // Signal is inverted
  time_delta = InternalClock() - get_reed_switch_first_press();

  if ((reed_switch1 || reed_switch2) && (get_release_timer() < InternalClock())){
    set_release_timer(InternalClock());            // Don't let release_timer have old time
  }
   
  // If magnet is present
  if ( reed_switch1 || reed_switch2 ) {
    waiting_to_be_retrieved = 0;                                                                                          // User has interacted - not waiting to be retrieved

    if ( time_delta > reed_switch_calibrate ) {
      while (!digitalRead(reed_switch_input1) ) {
        time_delta = InternalClock() - get_reed_switch_first_press();
        motor_forward();
        noInterrupts();
        if ( time_delta < reed_switch_super_long_press ){
          feedInnerWdt();
        }                                                  // Pet the watchdog while stuck in calibrate loop, stop if super long press to allow reboot
        delay(80);
        set_motor_position(0);                                                                                               // If calibrate is active, reset motor position to zero
        motor_off();
        interrupts();
        delay(500);
        waiting_to_be_retrieved = 1;
      }
    }
    else if ( time_delta > reed_switch_long_press ) {
      set_release_timer(InternalClock());                                                                                    // Reset timer if long press

    }
    else if ( time_delta > reed_switch_short_press ) {
      input_slowdown_toggle = !input_slowdown_toggle;                                                                     // Slow down adding time

      if (reed_switch1 && input_slowdown_toggle) {
        if (get_release_timer() < ( 60 + InternalClock())){
          set_release_timer(InternalClock() + release_timer_first_press_1);
        }
        else {
          set_release_timer(get_release_timer() + release_timer_add_1);
        }
      }
      else if (reed_switch2 && input_slowdown_toggle) {
        if (get_release_timer() < ( 60 + InternalClock())){
          set_release_timer(InternalClock() + release_timer_first_press_2);
        }
        else{
          set_release_timer(get_release_timer() + release_timer_add_2);
        }
      }
      // Reset the wiggle timer
      set_wiggle_timer(InternalClock() + wiggle_interval);
    }
  }
}

void am_i_waiting_to_be_recovered() {
  if(abs(get_motor_position()) < (open_position + 1000)){
    release_is_open = 1;
  }
  else if(abs(get_motor_position()) > (closed_position - 1000)){
    release_is_open = 0;
  }

  if ( reed_switch1 || reed_switch2 ){
    waiting_to_be_retrieved = 0;
  }
  else if((release_is_open == 1) && (release_last_position == 0)){
    waiting_to_be_retrieved = 1;
  }

  release_last_position = release_is_open;
  last_reed_switch_state = ( reed_switch1 || reed_switch2 );                                                                                   // Save state for next time
}

void debug_subroutine(void) {
  Serial.printf("Main Clock: %ld Time Until Release: %ld Encoder Time: %ld Encoder Power: %d Vext: %d\n"
  , InternalClock(), time_until_release, get_encoder_timer(), digitalRead(motor_driver_power), digitalRead(Vext));
}

// gets and sets
void set_battery_percent(int new_battery_percent){
  battery_percent = new_battery_percent;
}

int get_battery_percent(void){
  return battery_percent;
}
void set_sleep_inhibit(bool new_sleep_inhibit){
  sleep_inhibit = new_sleep_inhibit;
}

bool get_sleep_inhibit(void){
  return sleep_inhibit;
}
void set_display_active(bool new_display_active){
  display_active = new_display_active;
}

bool get_display_active(void){
  return display_active;
}
void set_reed_switch1(bool new_reed_switch1){
  reed_switch1 = new_reed_switch1;
}

bool get_reed_switch1(void){
  return reed_switch1;
}
void set_reed_switch2(bool new_reed_switch2){
  reed_switch2 = new_reed_switch2;
}

bool get_reed_switch2(void){
  return reed_switch2;
}
void set_last_reed_switch_state(bool new_last_reed_switch_state){
  last_reed_switch_state = new_last_reed_switch_state;
}

bool get_last_reed_switch_state(void){
  return last_reed_switch_state;
}
void set_waiting_to_be_retrieved(bool new_waiting_to_be_retrieved){
  waiting_to_be_retrieved = new_waiting_to_be_retrieved;
}

bool get_waiting_to_be_retrieved(void){
  return waiting_to_be_retrieved;
}
void set_release_is_open(bool new_release_is_open){
  release_is_open = new_release_is_open;
}

bool get_release_is_open(void){
  return release_is_open;
}
void set_release_last_position(bool new_release_last_position){
  release_last_position = new_release_last_position;
}

bool get_release_last_position(void){
  return release_last_position;
}
void set_is_led_activated(bool new_is_led_activated){
  is_led_activated = new_is_led_activated;
}

bool get_is_led_activated(void){
  return is_led_activated;
}
void set_gps_enabled(bool new_gps_enabled){
  gps_enabled = new_gps_enabled;
}

bool get_gps_enabled(void){
  return gps_enabled;
}
void set_gps_lock(bool new_gps_lock){
  gps_lock = new_gps_lock;
}

bool get_gps_lock(void){
  return gps_lock;
}
void set_input_slowdown_toggle(bool new_input_slowdown_toggle){
  input_slowdown_toggle = new_input_slowdown_toggle;
}

bool get_input_slowdown_toggle(void){
  return input_slowdown_toggle;
}