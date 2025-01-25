#include <Arduino.h>
#include <LoRa_APP.h>

#include "globals.h"
#include "motor.h"
#include "my_clock.h"
#include "subroutines.h"
#include "display.h"
#include "burd_radio.h"
#include "sleep.h"
#include "gps.h"
#include "watchdog.h"
#include "user_input.h"
#include "device_state.h"
#include "timers.h"
#include "battery.h"

void setup(){
  // put your setup code here, to run once:
  if(DEBUG){
    Serial.begin(115200);
  }
  
  TimerReset(0);
  boardInitMcu();                                                                                         // Hopefully reset onboard timers

  delay(100);

  if(DEBUG){
    Serial.printf("Booting up...\n");
  }

  // Disable interrupts for bootup
  noInterrupts();

  user_input_init();
  motor_init();

  // Setup GPS Power Control
  //pinMode(gps_power, OUTPUT);                               // This is controlled by Air530 library
  //gps_wake();
  //Air530.begin();
  //Air530.reset();
  //gps_sleep();                                              // Disable GPS module on boot
  //Serial.printf("GPS Initialized\n");

  // Enable interrupts
  interrupts();

  logo();

  // ROTATE SHAFT ON BOOT!!!
  motor_run_to_position(CLOSED_POSITION);
  delay(5000);
  motor_run_to_position(OPEN_POSITION);

  //Enable the WDT.

  // Star LoRa radio
  radio_init();

  VextOFF();

  init_sleep();

  go_to_sleep();
}

void update_time_until_release(){
  set_time_until_release(get_release_timer() - InternalClock());
  
  if(get_time_until_release() < 0){
    set_time_until_release(0);
  }
}

void loop(){
  if(DEBUG){
    Serial.printf("Loop Started\n");
  }

  feedInnerWdt();                                               // Pet the watchdog - interrupt generated every 1.4 seconds.  Two consecutive interrupts causes a reboot (2.8s)

  if(get_low_power()){
    lowPowerHandler();                                          //note that lowPowerHandler() runs six times before the mcu goes into low_power mode;
  }
  // Release is opened and closed in TimerWakeUp interrupt routine

  // Time until release
  update_time_until_release();
    
  // Update battery samples
  sampleBatteryVoltage();

  // Wiggle motor to break barnacles if release is closed
  if((get_wiggle_timer() < InternalClock()) && (get_release_is_open() == 0)) {
    // If we're more than wiggle_deadband seconds from opening the release, then allow a wiggle
    if(get_wiggle_timer() < (get_release_timer() - WIGGLE_DEADBAND)){
      wiggle_motor();
    }

    set_wiggle_timer(InternalClock() + WIGGLE_INTERVAL);
  }

  // Enable GPS if release is waiting to be retrieved
  if(GPS_ENABLE && get_waiting_to_be_retrieved() && ((InternalClock() - get_last_gps_fix()) > GPS_INTERVAL)) {
    //Serial.printf("Waking GPS.\n");
    gps_wake();                                               // This will only wake if GPS is asleep

    if (is_air_available() > 0) {
      if (DEBUG){
        Serial.printf("GPS Available.\n");
      }

      update_gps();
    }
  }
  else{
    gps_sleep();
  }
  
  // Power down encoder and motor driver if it's been a while since motor has been energized
  if(get_encoder_timer() < InternalClock()){
    motor_sleep();
  }

  // Send a LoRa packet
  if(LORA_ENABLE && get_waiting_to_be_retrieved() && ( get_lora_timer() < InternalClock())){
    if (DEBUG){
      Serial.printf("Sending LoRa packet.\n");
    }

    oled_wake();                                                                                     // Need to power up Vext to supply power to LoRa radio

    set_lora_timer(InternalClock() + LORA_INTERVAL);                                                                         // Use this for testing only
  }
  else {
    Radio.Sleep();                                                                                   // Put LoRa to sleep if we're not sending a packet
    //lora_timer = InternalClock() + lora_interval;                                                  // Increment this even if not tranmitting.  That way the LoRa radio won't transmit immediately upon activation (let float to surface) THIS BREAKS THINGS
  }

  // Draw OLED display
  int loop_counter = 0;
  set_timer_tap_multiplier1(0);                                                                          // Reset timer tap counter
  set_timer_tap_multiplier2(0);                                                                          // Reset timer tap counter

  long release_delta = get_release_timer() - InternalClock();         // Calculate difference from clock to set release time

  while((get_display_timer() > InternalClock()) || (abs(release_delta) < 10)) {
    release_delta = get_release_timer() - InternalClock();                                                 // Re-calculate difference from clock to set release time or it'll get stuck in display loop

    //VextON();                                                                                      // Power up Vext
    TimerWakeUp();                                                                                   // Call timer routine because it stops working when display loop is active
    set_display_active(1);

    if(loop_counter > 5){
      // Check reed switch input
      reed_switch_debounce();
    }                                                 

    update_display();
    delay(500);                                                                                      // This delay controls how fast time is added
    loop_counter++;
  }

  // Display notification screen if Waiting to be retrieved
  if((get_waiting_to_be_retrieved() == 1) && (get_display_active() == 0)){
    waiting_screen();
    delay(500);
  }

  // Flash LED or turn it off if not needed
  //if ( (waiting_to_be_retrieved == 1) && (display_active == 0) ) led_flasher();
  //else rgbpixel.clear(); // Set all pixel colors to 'off'

  //Make sure Vext is off
  if((get_waiting_to_be_retrieved == 0) && (get_display_active == 0)){
    VextOFF();
  }

  if(DEBUG){
    debug_subroutine();
  }

  // I have no idea what this does - something about being able to call an interrupt on completion of LoRa TX
  Radio.IrqProcess();  
                                                                               
  if(DEBUG){
    Serial.printf("Loop Ended, going to sleep.\n");
  }

  go_to_sleep();
}
