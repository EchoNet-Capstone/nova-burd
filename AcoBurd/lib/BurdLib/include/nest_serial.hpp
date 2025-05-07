#pragma once
#ifdef RECV_SERIAL_NEST
#include <stdint.h>

#include <HardwareSerial.h>

#include "device_actions.hpp"

#define NEST_SERIAL_CONNECTION Serial

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