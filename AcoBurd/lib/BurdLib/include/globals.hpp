#pragma once

#include <HardwareSerial.h>
#include <stdint.h>

#ifdef RECV_SERIAL_NEST
#define NEST_SERIAL_CONNECTION Serial
#endif

#define MODEM_SERIAL_CONNECTION Serial1

// Speed of sound in m/s
// #define SOUND_SPEED 343     // Sound speed through air
#define SOUND_SPEED 1500    // Sound speed through water