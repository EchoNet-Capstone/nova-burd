#ifndef DEVICE_ACTIONS_HPP
#define DEVICE_ACTIONS_HPP

#include <stdint.h>

struct
DeviceAction_t {
    uint8_t modemRespType;
    uint16_t srcAddr;
    uint8_t flocType;
    uint8_t commandType;
    uint8_t dataSize;
    uint8_t* data;
};

void
init_da(
    DeviceAction_t* da
);

void
act_upon(
    DeviceAction_t* da
);

#endif