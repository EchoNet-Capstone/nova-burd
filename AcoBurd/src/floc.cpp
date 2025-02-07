#include "floc.hpp"
#include <Arduino.h>




void floc_broadcast_received(char *broadcastBuffer) {

    struct header_common *type_ttl = (struct header_common *)broadcastBuffer;
    
    int8_t type = type_ttl->type;
    printf("Type is %d\r\n", type);

    switch (type) {
        case COMMAND_TYPE:
            if (debug) Serial.println("Command packet received");
            break;
        case ACK_TYPE:
            if (debug) Serial.println("Acknowledgement packet received");
            break;
        case RESPONSE_TYPE:
            if (debug) Serial.println("Response packet received");
            break;
    }

}

void floc_unicast_received() {

}