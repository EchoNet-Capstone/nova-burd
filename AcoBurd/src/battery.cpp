#include <stdint.h>
#include <Arduino.h>

#include "battery.h"
#include "timers.h"
#include "my_clock.h"
#include "device_state.h"

void VextON(){
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(){
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

// Measure battery voltage
void sampleBatteryVoltage(){
  if(get_battery_timer() >= InternalClock()){
    //Not time yet
    return;
  }

  set_battery_timer(InternalClock() + BATTERY_INTERVAL);

  noInterrupts();
  VextON();

  uint16_t volts = getBatteryVoltage();

  interrupts();                                                                         // Reenable interrupts after sample

  set_battery_percent((int)((100 * (volts - BATTERY_EMPTY)) / BATTERY_USABLE_VOLTS));
  if(get_battery_percent() > 100){
    set_battery_percent(100);
  }

  if(get_battery_percent() < 0){
    set_battery_percent(100);
  }

  if(get_battery_percent() < LOW_BATTERY){\
    // Release trap if battery gets low
    set_release_timer(InternalClock());
  }             
}