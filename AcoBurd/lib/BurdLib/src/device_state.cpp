#include <stdlib.h>

#include "get_set_macros.hpp"

#include "device_state.hpp"

GET_SET_FUNC_DEF(int, battery_percent, 0)
GET_SET_FUNC_DEF(bool, display_active, false)

// GPS Globals
GET_SET_FUNC_DEF(bool, gps_enabled, false)
GET_SET_FUNC_DEF(bool, gps_lock, false)
// Initial value hack needed to aquire GPS fix on bootup
GET_SET_FUNC_DEF(long, last_gps_fix, -3600)

//Sleep Globals
GET_SET_FUNC_DEF(bool, low_power, false)
GET_SET_FUNC_DEF(bool, sleep_inhibit, true)
GET_SET_FUNC_DEF(long, reed_switch_first_press, 0)
GET_SET_FUNC_DEF(int, timer_tap_multiplier1, 0)
GET_SET_FUNC_DEF(int, timer_tap_multiplier2, 0)

GET_SET_FUNC_DEF(bool, reed_switch1, false)
GET_SET_FUNC_DEF(bool, reed_switch2, false)
GET_SET_FUNC_DEF(bool, waiting_to_be_retrieved, false)
GET_SET_FUNC_DEF(bool, release_is_open, false)
GET_SET_FUNC_DEF(bool, release_last_position, false)