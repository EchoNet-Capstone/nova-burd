#pragma once

#include <stdint.h>

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

void
EEPROM_init(
    void
);