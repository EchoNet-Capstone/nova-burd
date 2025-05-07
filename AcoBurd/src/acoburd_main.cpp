#include <LoRaWan_APP.h>
#include <EEPROM.h>

// Kill it with fire
#include <safe_arduino.hpp>

#include <stdint.h>

#include <activity_period.hpp>
#include <buffer.hpp>
#include <burd_EEPROM.hpp>
#include <device_actions.hpp>
#include <display.hpp>
#include <motor.hpp>
#include <nest_serial.hpp>
#include <nmv3_service.hpp>
#include <services.hpp>

void
setup(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    // Debug messages to USB connection
    Serial.begin(115200, SERIAL_8N1);

    delay(100);
#endif // DEBUG_ON

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Booting up...\r\n");
#endif // DEBUG_ON

    EEPROM_firstTime();

    if(EEPROM_getDeviceIdNetworkIdSet() != 0x01) {
        // TODO: wait for device id's to come in through serial and display this on the device's screen
    
        EEPROM_setDeviceIdNetworkId((uint16_t) 0x0001, (uint16_t) 0x00001);
    }

#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
    nestSerial_init();
#endif // RECV_SERIAL_NEST

    nmv3_init();

    activitity_init();

    motor_init();

    display_init();

    registerAllServices();
}

void
loop(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    static bool print_start_loop = true;

    if(print_start_loop){
        Serial.printf(" Initialization Completed. Starting Services...\r\n");

        print_start_loop = false;
    }
#endif // DEBUG_ON
    
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
