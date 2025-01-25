#ifndef TIMERS_H
#define TIMERS_H

#include "get_set_macros.h"

// Timer Add Configuration
#define RELEASE_TIMER_ADD_1 7200              // Time to add for each magnet tap
#define RELEASE_TIMER_ADD_2 86400             // Time to add for each magnet tap
#define RELEASE_TIMER_FIRST_PRESS_1 120        // First tap starts clock at this number
#define RELEASE_TIMER_FIRST_PRESS_2 172830      // First tap starts clock at this number (add an extra 30 seconds for readability)
#define DISPLAY_TIMEOUT 20                    // Must be bigger than reset timer (15s)
#define ENCODER_TIMEOUT 10
#define BATTERY_INTERVAL 180                  // Time between battery samples
#define WIGGLE_INTERVAL 259200                // 86400 seconds in a day, 3 * 86400 = 259200
#define LORA_INTERVAL 60                      // Time between LoRa transmits

GET_SET_FUNC_PROTO(long, lora_timer);
GET_SET_FUNC_PROTO(long, display_timer);
GET_SET_FUNC_PROTO(long, encoder_timer);
GET_SET_FUNC_PROTO(long, release_timer);
GET_SET_FUNC_PROTO(long, release_timer1);
GET_SET_FUNC_PROTO(long, release_timer2);
GET_SET_FUNC_PROTO(long, battery_timer);
GET_SET_FUNC_PROTO(long, time_until_release);
GET_SET_FUNC_PROTO(long, wiggle_timer);

#endif
