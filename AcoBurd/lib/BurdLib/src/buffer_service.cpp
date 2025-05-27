#include "safe_arduino.hpp"

#include <floc_buffer.hpp>

#include "activity_period.hpp"
#include "services.hpp"

extern Service bufferServiceDesc;

void
bufferService(
    void
){
    bufferServiceDesc.busy = false;

    if (is_activity_period_open()) {
    // #ifdef DEBUG_ON // DEBUG_ON
    //     Serial.printf("Activity period is open for sending...\r\n");
    // #endif // DEBUG_ON

        if (flocBuffer.checkQueueStatus() == 0) {
        // #ifdef DEBUG_ON // DEBUG_ON
        //     Serial.printf("No packets in the queue...\r\n");
        // #endif // DEBUG_ON    

        } else {
        // #ifdef DEBUG_ON // DEBUG_ON
        //     Serial.printf("Packets in the queue...\r\n");
        // #endif // DEBUG_ON
        
            flocBuffer.queueHandler();

            bufferServiceDesc.busy = true;
        }
    }
}