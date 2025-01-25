#include "battery.h"
#include "timers.h"
#include "my_clock.h"
#include <stdint.h>

#include <Arduino.h>

// battery
int battery_percent = 0;

void VextON(){
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(){
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

// Measure battery voltage
uint16_t sampleBatteryVoltage(){
  noInterrupts();
  VextON();

  uint16_t volts = getBatteryVoltage();

  interrupts();                                                                         // Reenable interrupts after sample

  battery_percent = (int)((100 * (volts - battery_empty)) / battery_usable_volts);
  if(battery_percent > 100){
    battery_percent = 100;
  }

  if(battery_percent < 0){
    battery_percent = 0;
  }

  if(battery_percent < low_battery){\
    // Release trap if battery gets low
    set_release_timer(InternalClock());
  }             

  return volts;
}

// gets and sets
void set_battery_percent(int new_battery_percent){
  battery_percent = new_battery_percent;
}

int get_battery_percent(void){
  return battery_percent;
}