#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include "main.h"

void VextON(void);
void VextOFF(void);
void set_release_timer(void);
uint16_t sampleBatteryVoltage(void);
void reed_switch_debounce(void);
uint16_t sampleBatteryVoltage(void);
void reed_switch_debounce(void);
void am_i_waiting_to_be_recovered(void);

#endif