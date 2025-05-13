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
#include "safe_arduino.hpp"

#include <stdint.h>
#include <timer.h>

#include "activity_period.hpp"
#include "get_set_macros.hpp"
#include "services.hpp"

GET_SET_FUNC_DEF(activityState, activity_state, SENDING)

static unsigned long activity_period_start = 0;

const unsigned int ACTIVITY_PERIOD = 10000; // 10 second (total)
const unsigned int ACTIVITY_SENDING = 2000; // 2 seconds

extern Service activityServiceDesc;

void
activitity_init(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Activity Init...\r\n");
#endif // DEBUG_ON

    activity_period_start = millis();
    activity_state = SENDING;
}

void
activityService(
    void
){
    unsigned long current_time = millis();

    activityServiceDesc.busy = false;

    if ((current_time - activity_period_start < ACTIVITY_PERIOD)) {
        return;
    }

    switch(get_activity_state()) {
        case SENDING:
            if ((current_time - activity_period_start) >= ACTIVITY_SENDING) {
                activity_state = LISTENING;
                activity_period_start = current_time;

                activityServiceDesc.busy = true;
            }
            break;
        case LISTENING:
            if ((current_time - activity_period_start) >= ACTIVITY_PERIOD) {
                activity_state = SENDING;

                activityServiceDesc.busy = true;
            }
            break;
        default:
            break;
    }
}

bool
is_activity_period_open(
    void
){
    return get_activity_state() == SENDING;
}

