/* This is the activity period tracker
 * 
 * When the activity period is open, we are going to send the packets
 *  - 1
 * 
 * When the activity period is closed, we are waiting and recieving packets
 * 
 * Total activity period is 10 seconds
 * - 8 seconds listening
 * - 2 seconds sending
 * 
 * */

#include <stdint.h>
#include <stddef.h>
#include <timer.h>
#include <Arduino.h>

#include "activityperiod.hpp"

static activityState activity_state = LISTENING;

static unsigned long activity_period_start = 0;

const unsigned int ACTIVITY_PERIOD = 10000; // 10 second (total)
const unsigned int ACTIVITY_SENDING = 2000; // 2 seconds


void activitity_init(){
    activity_period_start = millis();
    activity_state = LISTENING;
}

void activity_update(){
    unsigned long current_time = millis();

    if ((activity_state == LISTENING) && (current_time - activity_period_start >= ACTIVITY_PERIOD)) {
        activity_state = SENDING;
        activity_period_start = current_time;
    } else if ((activity_state == SENDING) && (current_time - activity_period_start >= ACTIVITY_SENDING)) {
        activity_state = LISTENING;
        activity_period_start = current_time;
    }
}


enum activityState is_activity_period_open(){
    return activity_state;
}
