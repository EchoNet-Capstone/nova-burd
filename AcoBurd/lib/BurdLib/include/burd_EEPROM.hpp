#pragma once

#include <stdint.h>

const uint8_t SERIAL_NUM_SIZE       = sizeof(uint32_t);

const uint8_t MAGIC_SIZE            = sizeof(uint8_t);

const uint8_t DEVICE_ID_SIZE        = sizeof(uint16_t);
const uint8_t NETWORK_ID_SIZE       = sizeof(uint16_t);
const uint8_t DID_NID_SET_SIZE      = sizeof(uint8_t);

const uint8_t SERIAL_NUM_ADDR       = 0;

const uint8_t EEPROM_MAGIC          = 0x42;
const uint8_t MAGIC_ADDR            = SERIAL_NUM_ADDR + SERIAL_NUM_SIZE;

const uint8_t DEVICE_ID_ADDR        = MAGIC_ADDR + MAGIC_SIZE;
const uint8_t NETWORK_ID_ADDR       = DEVICE_ID_ADDR + NETWORK_ID_SIZE;
const uint8_t DID_NID_SET_BYTE_ADDR = NETWORK_ID_ADDR + DID_NID_SET_SIZE;

void
EEPROM_firstTime(
    void
);

void
EEPROM_clearSerialNumber(
    void
);

void
EEPROM_clearDeviceIdNetworkId(
    void
);

void
EEPROM_setSerialNumber(
    uint32_t new_SerialNumber
);

void
EEPROM_setDeviceIdNetworkId(
    uint16_t new_DeviceId,
    uint16_t new_NetworkId
);

uint8_t
EEPROM_getDeviceIdNetworkIdSet(
    void
);

uint32_t
EEPROM_getSerialNumber(
    void
);

uint16_t
EEPROM_getDeviceID(
    void
);

uint16_t
EEPROM_getNetworkID(
    void
);