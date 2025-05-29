#pragma once

#include <stdint.h>

#define SERIAL_NUM_SIZE       sizeof(uint32_t)

#define MAGIC_SIZE            sizeof(uint8_t)

#define DEVICE_ID_SIZE        sizeof(uint16_t)
#define NETWORK_ID_SIZE       sizeof(uint16_t)
#define DID_NID_SET_SIZE      sizeof(uint8_t)

#define EEPROM_SIZE           (size_t)( SERIAL_NUM_SIZE + \
                                MAGIC_SIZE + \
                                DEVICE_ID_SIZE + \
                                NETWORK_ID_SIZE + \
                                DID_NID_SET_SIZE ) 

#define SERIAL_NUM_ADDR       0

#define EEPROM_MAGIC          0x42
#define MAGIC_ADDR            ( SERIAL_NUM_ADDR + SERIAL_NUM_SIZE )

#define DEVICE_ID_ADDR        ( MAGIC_ADDR + MAGIC_SIZE )
#define NETWORK_ID_ADDR       ( DEVICE_ID_ADDR + DEVICE_ID_SIZE )
#define DID_NID_SET_BYTE_ADDR ( NETWORK_ID_ADDR + NETWORK_ID_SIZE )

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