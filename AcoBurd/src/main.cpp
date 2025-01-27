#include <Arduino.h>
#include <LoRa_APP.h>

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
  // Hopefully reset onboard timers
  boardInitMcu();

  delay(100);

  if(DEBUG){
    Serial.printf("Booting up...\n");
  }

  // Disable interrupts for bootup
  noInterrupts();

  user_input_init();

  motor_init();

  //gps_init();

  // Enable interrupts
  interrupts();

  logo();

  //Enable the WDT.
  innerWdtEnable(true);

  // Star LoRa radio
  radio_init();

  VextOFF();

  init_sleep();

  go_to_sleep();
}

void loop(){
  if(DEBUG){
    Serial.printf("Loop Started\n");
  }

  // Pet the watchdog - interrupt generated every 1.4 seconds.  Two consecutive interrupts causes a reboot (2.8s)
  feedInnerWdt();

  //note that lowPowerHandler() runs six times before the mcu goes into low_power mode;
  if(get_low_power()){
    lowPowerHandler();
  }

  release_service();

  battery_service();

  motor_service();

  gps_service();

  radio_service();

  interaction_service();

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
