#include "timers.h"
#include "get_set_macros.h"

// timer vars
GET_SET_DEF(long, lora_timer, 0);
GET_SET_DEF(long, display_timer, 0);
GET_SET_DEF(long, encoder_timer, 0);
GET_SET_DEF(long, release_timer, 0);
GET_SET_DEF(long, release_timer1, 0);
GET_SET_DEF(long, release_timer2, 0);
GET_SET_DEF(long, battery_timer, 0);
GET_SET_DEF(long, time_until_release, 0);
GET_SET_DEF(long, wiggle_timer, 0);