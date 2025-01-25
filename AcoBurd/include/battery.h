#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

#define battery_full 4050                     // Voltage for full battery - was 4200 (this resulted in full battery being 95%, 4140 is actual measured full voltage
#define battery_empty 3350                    // Voltage for empty battery - 3300 for nearly empty at 0C (https://www.richtek.com/Design%20Support/Technical%20Document/AN024)
#define battery_usable_volts (battery_full - battery_empty)
#define low_battery 5                         // Low battery condition in percent
#define battery_interval 180                  // Time between battery samples

void VextON(void);
void VextOFF(void);

uint16_t sampleBatteryVoltage(void);
void set_battery_percent(int new_battery_percent);
int get_battery_percent(void);

#endif