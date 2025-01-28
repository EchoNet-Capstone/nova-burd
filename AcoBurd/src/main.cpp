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
  Serial1.begin(9600, SERIAL_8N1);
  if(DEBUG){
    Serial.begin(115200);
  }

  TimerReset(0);
  // Hopefully reset onboard timers
  boardInitMcu();

  delay(100);

  if(DEBUG){
    Serial.printf("Booting up...\r\n");
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
  // Cooper Enabled here
  innerWdtEnable(true);

  // Star LoRa radio
  radio_init();

  VextOFF();

  init_sleep();

  go_to_sleep();
}

void loop(){


  if(DEBUG){
    if (Serial1.availableForWrite()) {
      Serial.println("Serial1 Available, transmitting");
      Serial1.write("$B04HEYO");
    }
    
    Serial.println("Loop Started");
    debug_subroutine();
  }

  // Pet the watchdog - interrupt generated every 1.4 seconds.  Two consecutive interrupts causes a reboot (2.8s)
  feedInnerWdt();

  //note that lowPowerHandler() runs six times before the mcu goes into low_power mode;
  if(get_low_power()){
    lowPowerHandler();
  }

  Serial.printf("Position 1\r\n");
  release_service();

  Serial.printf("Position 2\r\n");
  battery_service();

  Serial.printf("Position 3\r\n");
  motor_service();

  Serial.printf("Position 4\r\n");
  gps_service();

  Serial.printf("Position 5\r\n");
  radio_service();

  Serial.printf("Position 6\r\n");
  interaction_service();

  Serial.printf("Position 7\r\n");
  if(DEBUG){
    debug_subroutine();
  }

  // I have no idea what this does - something about being able to call an interrupt on completion of LoRa TX
  Radio.IrqProcess();

  Serial.printf("Position 8\r\n");
  if(DEBUG){
    Serial.printf("Loop Ended, going to sleep.\r\n");
  }

  go_to_sleep();
}
