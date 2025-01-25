#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

#include "get_set_macros.h"

GET_SET_FUNC_PROTO(int, battery_percent);
GET_SET_FUNC_PROTO(bool, display_active);
GET_SET_FUNC_PROTO(bool, gps_enabled);
GET_SET_FUNC_PROTO(bool, gps_lock);
GET_SET_FUNC_PROTO(long, last_gps_fix);

GET_SET_FUNC_PROTO(int, motor_position);

GET_SET_FUNC_PROTO(bool, low_power);
GET_SET_FUNC_PROTO(bool, sleep_inhibit);
GET_SET_FUNC_PROTO(long, reed_switch_first_press);
GET_SET_FUNC_PROTO(int, timer_tap_multiplier1);
GET_SET_FUNC_PROTO(int, timer_tap_multiplier2);

GET_SET_FUNC_PROTO(bool, reed_switch1);
GET_SET_FUNC_PROTO(bool, reed_switch2);
GET_SET_FUNC_PROTO(bool, waiting_to_be_retrieved);
GET_SET_FUNC_PROTO(bool, release_is_open);
GET_SET_FUNC_PROTO(bool, release_last_position);

void am_i_waiting_to_be_recovered(void);
#endif