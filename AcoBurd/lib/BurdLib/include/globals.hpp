#pragma once

#include <stdint.h>

#define ON_DEVICE

#define NEST_SERIAL_CONNECTION Serial
#define MODEM_SERIAL_CONNECTION Serial1

// Speed of sound in m/s
// #define SOUND_SPEED 343     // Sound speed through air
#define SOUND_SPEED 1500    // Sound speed through water

static inline
uint16_t
htons(
    uint16_t val
){
    return __builtin_bswap16(val);
}

static inline
uint16_t
ntohs(
    uint16_t val
){
    return __builtin_bswap16(val);
}