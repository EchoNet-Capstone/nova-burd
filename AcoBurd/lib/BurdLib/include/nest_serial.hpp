#pragma once
#ifdef RECV_SERIAL_NEST
#include <stdint.h>

void
packet_received_nest(
    uint8_t* packetBuffer,
    uint8_t size
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