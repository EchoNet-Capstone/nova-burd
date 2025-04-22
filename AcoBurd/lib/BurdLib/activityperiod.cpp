/* This is the activity period tracker
 * 
 * When the activity period is open, we are going to send the packets
 *  - 1
 * 
 * When the activity period is closed, we are waiting and recieving packets
 * 
 * 
 * */

#include <stdint.h>
#include <stddef.h>
#include <timer.h>
#include <Arduino.h>

#include "activityperiod.hpp"

static activityState activity_state = LISTENING;

static unsigned long activity_period_start = 0;

const unsigned int ACTIVITY_PERIOD = 10000; // 10 second
const unsigned int ACTIVITY_SENDING = 5000; // 5 seconds


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










 /*
  * The Activity Period shall be a total of ten (10) seconds, described by the following:
Sending BuRD to send and receive a 64-byte FLOC packet
1.105 seconds * 2 = 2.21 seconds
Receiving BuRD to send and receive a 64-byte FLOC packet
1.105 seconds * 2 = 2.21 seconds
Maximum round trip time
2 seconds * 2 = 4 seconds
Buffer time for processing
1.58 seconds
The Activity Period shall be used for all FLOC packet transmissions, for all BuRDs.
The Activity Period shall not be prematurely ended.
  * 
  * 
  * */