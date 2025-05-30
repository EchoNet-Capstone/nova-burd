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

static uint32_t activity_period_start = 0;

#define ACTIVITY_PERIOD  10000  // 10 second (total)
#define ACTIVITY_SENDING  2000  // 2 seconds

extern Service activityServiceDesc;

bool
is_activity_period_open(
    void
){
    return get_activity_state() == SENDING;
}

void
activitity_init(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Activity Init with jitter...\r\n");
#endif // DEBUG_ON

    activity_period_start = millis() + random(0, 2000);

    activity_state = SENDING;
}

void
activityService(
    void
){
    uint32_t current_time = millis();

    activityServiceDesc.busy = false;

    if( current_time < activity_period_start ){
        /* Do Nothing */

        return;
    }

    switch(get_activity_state()) {
        case SENDING:
            if ((current_time - activity_period_start) >= ACTIVITY_SENDING) {
                set_activity_state(LISTENING);

                activityServiceDesc.busy = true;

            }
            break;
        case LISTENING:
            if ((current_time - activity_period_start) >= ACTIVITY_PERIOD) {
                set_activity_state(SENDING);


                activity_period_start = current_time + random(0, 2000);
                activityServiceDesc.busy = true;
            }
            break;
        case RANGING:
            /* Do nothing, neighbor service does this stuff */

            activityServiceDesc.busy = true;
            break;
        default:
            break;
    }
}
