#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

#define BATTERY_FULL 4050                     // Voltage for full battery - was 4200 (this resulted in full battery being 95%, 4140 is actual measured full voltage
#define BATTERY_EMPTY 3350                    // Voltage for empty battery - 3300 for nearly empty at 0C (https://www.richtek.com/Design%20Support/Technical%20Document/AN024)
#define BATTERY_USABLE_VOLTS (BATTERY_FULL - BATTERY_EMPTY)
#define LOW_BATTERY 5                         // Low battery condition in percent


void VextON(void);
void VextOFF(void);

void sampleBatteryVoltage(void);

#endif