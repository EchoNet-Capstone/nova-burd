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

    if (!is_activity_period_open()) {
        /* Do Nothing */
        return;
    }

    if (flocBuffer.checkQueueStatus() != 0) {
        flocBuffer.queueHandler();

        bufferServiceDesc.busy = true;

        return;
    }

    /* Do Nothing */
}