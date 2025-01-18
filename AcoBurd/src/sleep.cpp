#include "sleep.hpp"
#include "subroutines.hpp"
#include "display.hpp"
#include "motor.hpp"
#include "globals.hpp"
#include "watchdog.hpp"
#include "LoRa_APP.h"
#include <Arduino.h>

// Interrupt Timer Configuration
#define timetillwakeup 1000                   // Cannot be longer than 1.4 seconds due to WDT

TimerEvent_t wakeUp;
bool low_power;

long reed_switch_first_press = 0;
long reed_switch_release_time = 0;
int timer_tap_multiplier1 = 0;
int timer_tap_multiplier2 = 0;
int wait_screen_delay = 0;

void set_reed_switch_first_press(long new_reed_switch_first_press){
  reed_switch_first_press = new_reed_switch_first_press;
}

long get_reed_switch_first_press(void){
  return reed_switch_first_press;
}
void set_reed_switch_release_time(long new_reed_switch_release_time){
  reed_switch_release_time = new_reed_switch_release_time;
}

long get_reed_switch_release_time(void){
  return reed_switch_release_time;
}
void set_timer_tap_multiplier1(int new_timer_tap_multiplier1){
  timer_tap_multiplier1 = new_timer_tap_multiplier1;
}

int get_timer_tap_multiplier1(void){
  return timer_tap_multiplier1;
}
void set_timer_tap_multiplier2(int new_timer_tap_multiplier2){
  timer_tap_multiplier2 = new_timer_tap_multiplier2;
}

int get_timer_tap_multiplier2(void){
  return timer_tap_multiplier2;
}
void set_wait_screen_delay(int new_wait_screen_delay){
  wait_screen_delay = new_wait_screen_delay;
}

int get_wait_screen_delay(void){
  return wait_screen_delay;
}

void init_sleep(){
  TimerInit(&wakeUp, TimerWakeUp);
}

void TimerWakeUp(){
  if(debug){
    Serial.printf("Woke up at %ld s.  Countdown timer %ld.\n", InternalClock(), get_release_timer());
  }

  low_power = false;
  feedInnerWdt();           // Pet the watchdog

  // Is release open?
  am_i_waiting_to_be_recovered();

  set_motor_state();                                                                                    // This opens and closes the release based on time

  // Flash LED or turn it off if not needed
  if(led_enable){
    if((get_waiting_to_be_retrieved() == 1) && (get_display_active() == 0)){
      led_flasher();
    }
    else{
      // Set all pixel colors to 'off'
      rgb_led(0, 0, 0);
    } 
  }
}

void go_to_sleep(){
  // Prevent CPU sleep if GPS is active
  if (get_gps_enabled() == 1){
    set_sleep_inhibit(1);
  }
  else{
    set_sleep_inhibit(0);
  }

  if(!get_sleep_inhibit()){
    low_power = true;
  }

  if(get_sleep_inhibit()){
    low_power = false;
  }

  set_display_active(0);
  motor_sleep();
  oled_sleep();

  if((get_waiting_to_be_retrieved == 0)){
    VextOFF();                  // Turn off Vext only if release is not waiting, otherwise supply power to LED
  }

  // If allowed to sleep, then set timer
  if(!get_sleep_inhibit()){
    Radio.Sleep( );                                                 // Power down radio if sleep is allowed
    TimerSetValue( &wakeUp, timetillwakeup );
    TimerStart( &wakeUp );
  }
  else{
    delay(timetillwakeup);                                          // Just wait around for a while if sleep is inhibited
    TimerWakeUp();
  }
}

// Reed switch interrupt
void gpio_interrupt(){
  low_power = false;
  set_reed_switch1(!digitalRead(reed_switch_input1));                                                          // Signal is inverted
  set_reed_switch2(!digitalRead(reed_switch_input2));                                                          // Signal is inverted

  noInterrupts();                                                                                         // Disable interrupts or motor won't spin
  if(debug){
    Serial.printf("GPIO Interrupt at %d ms.\n", InternalClock() );
  }

  reed_switch_first_press = InternalClock();                                                              // new press, record first time of key press
  am_i_waiting_to_be_recovered();
  set_display_timer(InternalClock() + display_timeout);                                                      // Activate display
  //delay(500);                                                                                           // Add small delay to allow switch to settle - THIS BREAKS THINGS
  interrupts();
}

bool is_low_power(){
  return low_power;
}
