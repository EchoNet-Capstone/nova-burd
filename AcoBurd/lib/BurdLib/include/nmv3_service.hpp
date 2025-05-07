#pragma once

#include <stdint.h>

#include <HardwareSerial.h>

#define MODEM_SERIAL_CONNECTION Serial1

void
modemService(
    void
);

void
nmv3_init(
    void
);