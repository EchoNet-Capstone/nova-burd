#include "LoRaWan_APP.h"
#include <EEPROM.h>

// Kill it with fire
#include <safe_arduino.hpp>

#include <stdint.h>

#include <activity_period.hpp>
#include <buffer.hpp>
#include <device_actions.hpp>
#include <display.hpp>
#include <globals.hpp>
#include <motor.hpp>
#include <nmv3_service.hpp>
#include <services.hpp>

void
setup(
    void
){
    uint8_t magic;
    EEPROM.get(MAGIC_ADDR, magic);

    if(magic != EEPROM_MAGIC) {
        // Initialize EEPROM
        EEPROM.put(MAGIC_ADDR, (uint8_t) EEPROM_MAGIC);
        EEPROM.put(DEVICE_ID_ADDR, (uint16_t) 0x0000);
        EEPROM.put(NETWORK_ID_ADDR, (uint16_t) 0x0000);
        EEPROM.put(SET_BYTE_ADDR, (uint8_t) 0x00);
    }

    // Debug messages to USB connection
    NEST_SERIAL_CONNECTION.begin(115200, SERIAL_8N1);

    // Serial connection to modem
    MODEM_SERIAL_CONNECTION.begin(9600, SERIAL_8N1);

    delay(100);

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Booting up...\r\n");
#endif // DEBUG_ON

    uint8_t set;
    EEPROM.get(SET_BYTE_ADDR, set);

    if(set != 0x01) {
        // TODO: wait for device id's to come in through serial and display this on the device's screen

        EEPROM.put(DEVICE_ID_ADDR, (uint16_t) 0x0001);
        EEPROM.put(NETWORK_ID_ADDR, (uint16_t) 0x0001);
        EEPROM.put(SET_BYTE_ADDR, (uint8_t) 0x01);
    }

    uint16_t t_device_id;
    uint16_t t_network_id;

    EEPROM.get(DEVICE_ID_ADDR, t_device_id);
    EEPROM.get(NETWORK_ID_ADDR, t_network_id);

    set_device_id(t_device_id);
    set_network_id(t_network_id);

    noInterrupts();

    motor_init();

    nmv3_init();

    oled_initialize();

    activitity_init();

    interrupts();

    registerAllServices();
}

void
loop(
    void
){
    // we are going to have a command activitiy variable

    auto& svcs = ServiceRegistry::instance().services();
    uint32_t now = millis();
    bool anyBusy = false;

    for (auto* s : svcs) {
        s->busy = false;
        s->fn();
        anyBusy |= s->busy;
    }

    if (!anyBusy) {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Going to sleep...\r\n");
    #endif
        LoRaWAN.sleep();
    }
}
