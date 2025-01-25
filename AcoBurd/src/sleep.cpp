#include <Arduino.h>
#include <LoRa_APP.h>

#include "sleep.h"
#include "subroutines.h"
#include "display.h"
#include "motor.h"
#include "globals.h"
#include "device_state.h"
#include "battery.h"
#include "my_clock.h"
#include "watchdog.h"
#include "timers.h"
#include "user_input.h"

TimerEvent_t wakeUp;

void init_sleep(){
  TimerInit(&wakeUp, TimerWakeUp);
}

void TimerWakeUp(){
  if(DEBUG){
    Serial.printf("Woke up at %ld s.  Countdown timer %ld.\n", InternalClock(), get_release_timer());
  }

  set_low_power(false);
  feedInnerWdt();           // Pet the watchdog

  // Is release open?
  am_i_waiting_to_be_recovered();

  set_motor_state();                                                                                    // This opens and closes the release based on time

  // Flash LED or turn it off if not needed
  if(LED_ENABLE){
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
  if(get_gps_enabled() == 1){
    set_sleep_inhibit(1);
  }
  else{
    set_sleep_inhibit(0);
  }

  if(!get_sleep_inhibit()){
    set_low_power(true);
  }

  if(get_sleep_inhibit()){
    set_low_power(false);
  }

  set_display_active(0);
  motor_sleep();
  oled_sleep();

  if((get_waiting_to_be_retrieved == 0)){
    VextOFF();                  // Turn off Vext only if release is not waiting, otherwise supply power to LED
  }

  // If allowed to sleep, then set timer
  if(!get_sleep_inhibit()){
    Radio.Sleep();                                                 // Power down radio if sleep is allowed
    TimerSetValue(&wakeUp, TIMETILLWAKEUP);
    TimerStart(&wakeUp);
  }
  else{
    delay(TIMETILLWAKEUP);                                          // Just wait around for a while if sleep is inhibited
    TimerWakeUp();
  }
}

// Reed switch interrupt
void gpio_interrupt(){
  set_low_power(false);
  set_reed_switch1(!digitalRead(REED_SWITCH_INPUT1));                                                          // Signal is inverted
  set_reed_switch2(!digitalRead(REED_SWITCH_INPUT2));                                                          // Signal is inverted

  noInterrupts();                                                                                         // Disable interrupts or motor won't spin
  if(DEBUG){
    Serial.printf("GPIO Interrupt at %d ms.\n", InternalClock() );
  }

  set_reed_switch_first_press(InternalClock());                                                              // new press, record first time of key press
  am_i_waiting_to_be_recovered();
  set_display_timer(InternalClock() + DISPLAY_TIMEOUT);                                                      // Activate display
  //delay(500);                                                                                           // Add small delay to allow switch to settle - THIS BREAKS THINGS
  interrupts();
}