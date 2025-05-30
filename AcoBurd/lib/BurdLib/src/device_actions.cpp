#include "safe_arduino.hpp"

#include <stdint.h>

#include <floc.hpp>

#include "device_state.hpp"
#include "display.hpp"
#include "motor.hpp"

#include "device_actions.hpp"

DeviceAction_t da = {0};

void
execute_command_1(
    uint8_t* data,
    uint8_t dataSize
){
    // TODO : Code to release buoy goes here
    // motor_run_to_position(CLOSED_POSITION);
}

void
execute_command_2(
    uint16_t srcAddr,
    uint8_t* data,
    uint8_t dataSize
){
    // TODO : Process command type 2
    floc_status_query(srcAddr);
}

void
act_upon_data(
    uint8_t* data,
    uint8_t dataSize
){
    // TODO : Handle data packet processing
}

void
act_upon_command(
    uint16_t srcAddr,
    uint8_t commandType,
    uint8_t* data,
    uint8_t dataSize
){
    switch(commandType){
        case COMMAND_TYPE_1:
            execute_command_1(data, dataSize);
            break;
        case COMMAND_TYPE_2:
            execute_command_2(srcAddr, data, dataSize);
            break;
        // ...

        default:
            // ERROR: unhandled command type
            break;
    }
}

void
act_upon_ack(
    uint16_t srcAddr
){
    // TODO : Handle ACK processing (e.g., mark packet as acknowledged)
}

void
act_upon_response(
    uint16_t* srcAddr,
    uint8_t* data,
    uint8_t dataSize
){
    // TODO : Handle response data processing
}

void
act_upon(
    void
){
    switch(da.flocType){
        case FLOC_DATA_TYPE:
            break;
        case FLOC_COMMAND_TYPE:
            act_upon_command(da.srcAddr, da.commandType, da.data, da.dataSize);
            break;
        case FLOC_ACK_TYPE:
            break;
        case FLOC_RESPONSE_TYPE:
            break;

        default:
            // ERROR invalid type
            break;
    }
    return;
}