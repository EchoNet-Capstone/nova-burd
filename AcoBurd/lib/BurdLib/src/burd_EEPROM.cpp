#include <EEPROM.h>
#include "safe_arduino.hpp"

#include <stdint.h>

#include <floc.hpp>

#include "burd_EEPROM.hpp"

void
EEPROM_firstTime(
    void
){
    uint8_t magic;
    EEPROM.get(MAGIC_ADDR, magic);

    if(magic != EEPROM_MAGIC) {
        // Initialize EEPROM
        EEPROM.put(MAGIC_ADDR, (uint8_t) EEPROM_MAGIC);

        EEPROM_clearDeviceIdNetworkId();
    }
}

void
EEPROM_clearSerialNumber(
    void
){
    EEPROM.put(SERIAL_NUM_ADDR, (uint32_t) 0x00000000);
}

void
EEPROM_clearDeviceIdNetworkId(
    void
){
    EEPROM.put(DEVICE_ID_ADDR, (uint16_t) 0x0000);
    EEPROM.put(NETWORK_ID_ADDR, (uint16_t) 0x0000);

    EEPROM.put(DID_NID_SET_BYTE_ADDR, (uint8_t) 0x00);
}

void
EEPROM_setSerialNumber(
    uint32_t new_SerialNumber
){
    EEPROM.put(SERIAL_NUM_ADDR, new_SerialNumber);
}

void
EEPROM_setDeviceIdNetworkId(
    uint16_t new_deviceId,
    uint16_t new_networkId
){
    EEPROM.put(DEVICE_ID_ADDR, new_deviceId);
    EEPROM.put(NETWORK_ID_ADDR, new_networkId);

    EEPROM.put(DID_NID_SET_BYTE_ADDR, (uint8_t) 0x01);
}

uint8_t
EEPROM_getDeviceIdNetworkIdSet(
    void
){
    uint8_t set;
    EEPROM.get(DID_NID_SET_BYTE_ADDR, set);

    return set;
}

uint32_t
EEPROM_getSerialNumber(
    void
){
    uint32_t ser;
    EEPROM.get(SERIAL_NUM_ADDR, ser);

    return ser;
}

uint16_t
EEPROM_getDeviceID(
    void
){
    uint16_t dId;
    EEPROM.get(DEVICE_ID_ADDR, dId);

    return dId;
}

uint16_t
EEPROM_getNetworkID(
    void
){
    uint16_t nId;
    EEPROM.get(NETWORK_ID_ADDR, nId);

    return nId;
}

void
EEPROM_init(
    void
){
    EEPROM.begin(EEPROM_SIZE);

    EEPROM_firstTime();

    uint8_t dIDnIDSet = EEPROM_getDeviceIdNetworkIdSet();

    if (dIDnIDSet != 0x01)
    {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Device ID and Network ID not set in EEPROM. Setting...\r\n");
    #endif // DEBUG_ON

        // TODO: wait for device id's to come in through serial and display this on the device's screen
    #ifdef RECV_SERIAL_NEST
        EEPROM_setDeviceIdNetworkId((uint16_t)0x0001, (uint16_t)0x0001);
    #else
        EEPROM_setDeviceIdNetworkId((uint16_t)0x0002, (uint16_t)0x0001);
    #endif
    } 

    
    uint16_t t_device_id = EEPROM_getDeviceID();
    uint16_t t_network_id = EEPROM_getNetworkID();

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Got DID (%d), NID (%d) from EEPROM. Setting locally...\r\n", t_device_id, t_network_id);
#endif // DEBUG_ON

    set_device_id(t_device_id);
    set_network_id(t_network_id);

    // End interaction with EEPROM. Commit changes to flash.
    EEPROM.end();
}