#pragma once

#include <HardwareSerial.h>
#include <stdint.h>

#define NEST_SERIAL_CONNECTION Serial
#define MODEM_SERIAL_CONNECTION Serial1

const uint8_t EEPROM_MAGIC = 0x42;
const int MAGIC_ADDR = 0;

const int DEVICE_ID_ADDR = 1;
const int NETWORK_ID_ADDR = 3;
const int SET_BYTE_ADDR = 5;

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