#include <Arduino.h>

#include "user_input.h"
#include "timers.h"
#include "motor.h"
#include "globals.h"
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

void reed_switch_debounce(){
  long time_delta;

  set_reed_switch1(!digitalRead(REED_SWITCH_INPUT1)); // Signal is inverted
  set_reed_switch2(!digitalRead(REED_SWITCH_INPUT2)); // Signal is inverted
  time_delta = InternalClock() - get_reed_switch_first_press();

  if((get_reed_switch1() || get_reed_switch2()) && (get_release_timer() < InternalClock())){
    // Don't let release_timer have old time

    set_release_timer(InternalClock());
  }

  // If magnet is present
  if(get_reed_switch1() || get_reed_switch2()){
    // User has interacted - not waiting to be retrieved
    set_waiting_to_be_retrieved(false);

    if(time_delta > REED_SWITCH_CALIBRATE){
      while(!digitalRead(REED_SWITCH_INPUT1)){
        time_delta = InternalClock() - get_reed_switch_first_press();
        motor_forward();
        noInterrupts();
        if ( time_delta < REED_SWITCH_SUPER_LONG_PRESS ){
          feedInnerWdt();
        }                                                  // Pet the watchdog while stuck in calibrate loop, stop if super long press to allow reboot
        delay(80);
        // If calibrate is active, reset motor position to zero
        set_motor_position(0);
        motor_off();
        interrupts();
        delay(500);
        set_waiting_to_be_retrieved(true);
      }
    }
    else if(time_delta > REED_SWITCH_LONG_PRESS){
      // Reset timer if long press
      set_release_timer(InternalClock());
    }
    else if(time_delta > REED_SWITCH_SHORT_PRESS){
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
  }
}