#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <stdio.h>

void VextON(void);
void VextOFF(void);
uint16_t sampleBatteryVoltage(void);
void reed_switch_debounce(void);
uint16_t sampleBatteryVoltage(void);
void reed_switch_debounce(void);
void am_i_waiting_to_be_recovered(void);
void debug_subroutine(void);

void set_motor_state(void);

// ahh p2?
void set_battery_percent(int new_battery_percent);
int get_battery_percent(void);

void set_sleep_inhibit(bool new_sleep_inhibit);
bool get_sleep_inhibit(void);

void set_display_active(bool new_display_active);
bool get_display_active(void);

void set_reed_switch1(bool new_reed_switch1);
bool get_reed_switch1(void);

void set_reed_switch2(bool new_reed_switch2);
bool get_reed_switch2(void);

void set_last_reed_switch_state(bool new_last_reed_switch_state);
bool get_last_reed_switch_state(void);

void set_waiting_to_be_retrieved(bool new_waiting_to_be_retrieved);
bool get_waiting_to_be_retrieved(void);

void set_release_is_open(bool new_release_is_open);
bool get_release_is_open(void);

void set_release_last_position(bool new_release_last_position);
bool get_release_last_position(void);

void set_is_led_activated(bool new_is_led_activated);
bool get_is_led_activated(void);

void set_gps_enabled(bool new_gps_enabled);
bool get_gps_enabled(void);

void set_gps_lock(bool new_gps_lock);
bool get_gps_lock(void);

void set_input_slowdown_toggle(bool new_input_slowdown_toggle);
bool get_input_slowdown_toggle(void);

#endif