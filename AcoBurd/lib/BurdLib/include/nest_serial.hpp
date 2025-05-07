#pragma once
#ifdef RECV_SERIAL_NEST
#include <stdint.h>

#include "device_actions.hpp"

void
packet_received_nest(
    uint8_t* packetBuffer,
    uint8_t size,
    DeviceAction_t* da
);

void
nestSerialService(
    void
);

void
nestSerial_init(
    void
);
#endif