#include <Arduino.h>

#include "user_input.h"
#include "display.h"
#include "timers.h"
#include "motor.h"
#include "sleep.h"
#include "device_state.h"
#include "my_clock.h"
#include "watchdog.h"

GET_SET_DEF(bool, input_slowdown_toggle, false);

void user_input_init(void){
  //pinMode(INT_GPIO, INPUT);
  if(HALL_EFFECT){
    pinMode(REED_SWITCH_INPUT1, INPUT);
    pinMode(REED_SWITCH_INPUT2, INPUT);
  }
  else{
    // Internal pull-up is 4.7k --maybe
    pinMode(REED_SWITCH_INPUT1, INPUT_PULLUP);
    pinMode(REED_SWITCH_INPUT2, INPUT_PULLUP);
  }

  attachInterrupt(REED_SWITCH_INPUT1, gpio_interrupt, FALLING);
  attachInterrupt(REED_SWITCH_INPUT2, gpio_interrupt, FALLING);
}

void interaction_service(){
  int loop_counter = 0;
  // Reset timer tap counter
  set_timer_tap_multiplier1(0);
  // Reset timer tap counter
  set_timer_tap_multiplier2(0);

  // Calculate difference from clock to set release time
  long release_delta = get_release_timer() - InternalClock();

  while((get_display_timer() > InternalClock()) || (abs(release_delta) < 10)) {
    // Re-calculate difference from clock to set release time or it'll get stuck in display loop
    release_delta = get_release_timer() - InternalClock();

    // Power up Vext
    //VextON();
    // Call timer routine because it stops working when display loop is active
    TimerWakeUp();
    set_display_active(1);

    if(loop_counter > 5){
      // Check reed switch input
      reed_switch_debounce();
    }

    update_display();
    // This delay controls how fast time is added
    delay(500);
    loop_counter++;
  }

  // Display notification screen if Waiting to be retrieved
  if((get_waiting_to_be_retrieved() == true) && (get_display_active() == false)){
    waiting_screen();
    delay(500);
  }
}

void reed_switch_calibrate(long* time_delta){
  while(!digitalRead(REED_SWITCH_INPUT1)){
    *time_delta = InternalClock() - get_reed_switch_first_press();

    motor_forward();
    noInterrupts();

    if(*time_delta < REED_SWITCH_SUPER_LONG_PRESS){
      // Pet the watchdog while stuck in calibrate loop, stop if super long press to allow reboot
      feedInnerWdt();
    }

    delay(80);

    // If calibrate is active, reset motor position to zero
    set_motor_position(0);
    motor_off();

    interrupts();

    delay(500);
    set_waiting_to_be_retrieved(true);
  }
}

void reed_switch_short_press(){
  // Slow down adding time
  set_input_slowdown_toggle(!input_slowdown_toggle);

  if(get_reed_switch1() && input_slowdown_toggle){
    if (get_release_timer() < ( 60 + InternalClock())){
      set_release_timer(InternalClock() + RELEASE_TIMER_FIRST_PRESS_1);
    }
    else{
      set_release_timer(get_release_timer() + RELEASE_TIMER_ADD_1);
    }
  }
  else if(get_reed_switch2() && input_slowdown_toggle){
    if (get_release_timer() < ( 60 + InternalClock())){
      set_release_timer(InternalClock() + RELEASE_TIMER_FIRST_PRESS_2);
    }
    else{
      set_release_timer(get_release_timer() + RELEASE_TIMER_ADD_2);
    }
  }

  // Reset the wiggle timer
  set_wiggle_timer(InternalClock() + WIGGLE_INTERVAL);
}

void reed_switch_debounce(){
  set_reed_switch1(!digitalRead(REED_SWITCH_INPUT1)); // Signal is inverted
  set_reed_switch2(!digitalRead(REED_SWITCH_INPUT2)); // Signal is inverted

  long time_delta = InternalClock() - get_reed_switch_first_press();

  if((get_reed_switch1() || get_reed_switch2()) && (get_release_timer() < InternalClock())){
    // Don't let release_timer have old time
    set_release_timer(InternalClock());
  }

  // If magnet is present
  if(!get_reed_switch1() && !get_reed_switch2()){
    return;
  }

  // User has interacted - not waiting to be retrieved
  set_waiting_to_be_retrieved(false);

  if(time_delta > REED_SWITCH_CALIBRATE){
    reed_switch_calibrate(&time_delta);
  }
  else if(time_delta > REED_SWITCH_LONG_PRESS){
    // Reset timer if long press
    set_release_timer(InternalClock());
  }
  else if(time_delta > REED_SWITCH_SHORT_PRESS){
    reed_switch_short_press();
  }
}