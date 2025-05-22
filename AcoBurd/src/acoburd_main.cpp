#include <stdint.h>

#include <safe_arduino.hpp>

#include <activity_period.hpp>
#include <buffer_service.hpp>
#include <burd_EEPROM.hpp>
#include <device_actions.hpp>
#include <display.hpp>
#include <lora_service.hpp>
#include <motor.hpp>
#include <nest_serial.hpp>
#include <nmv3_service.hpp>
#include <services.hpp>
#include <sleep.hpp>

void
setup(
    void
){
    boardInitMcu();

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
    #ifdef RECV_SERIAL_NEST
        EEPROM_setDeviceIdNetworkId((uint16_t) 0x0001, (uint16_t) 0x0001);
    #else
        EEPROM_setDeviceIdNetworkId((uint16_t) 0x0002, (uint16_t) 0x0001);
    #endif
    }

#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
    nestSerial_init();
#endif // RECV_SERIAL_NEST

    nmv3_init();

    // lora_init();

    activitity_init();

#ifndef RECV_SERIAL_NEST // !RECV_SERIAL_NEST
    motor_init();
#endif // !RECV_SERIAL_NEST

    display_init();

    registerAllServices();

#ifdef DEBUG_ON // DEBUG_ON
    delay(1000);
#endif
}

extern volatile bool sleep_requested;

void
loop(
    void
){
    static int noBusy = 0;
    static bool in_sleep = false;

    if ( sleep_requested && in_sleep ){ // waiting for MCU to actually go to sleep
        return;
    } else if ( sleep_requested && !in_sleep ){ // we've asked to sleep, but we haven't told the device yet
        in_sleep = true;
        goToSleep();
        
        return;
    } else if ( !sleep_requested && in_sleep ){ // we've woken up from sleep, execute wakeup
        wakeUp();
    } else {
        // Normal Operation
    }

    in_sleep = sleep_requested;

#ifdef DEBUG_ON // DEBUG_ON
    static bool print_start_loop = true;

    if(print_start_loop){
        Serial.printf("Initialization Completed. Starting Services...\r\n");

        print_start_loop = false;
    }
#endif // DEBUG_ON
    
    auto& svcs = ServiceRegistry::instance().services();
    uint32_t now = millis();
    bool anyBusy = false;

    for (auto* s : svcs) {
        s->busy = false;
        s->fn();
        anyBusy |= s->busy;
    }

#ifndef RECV_SERIAL_NEST // !RECV_SERIAL_NEST
    if (!anyBusy) {
        noBusy++;

        if(noBusy > 100){
            noBusy = 0;
            sleep_requested = true;
        }
    }
#endif // !RECV_SERIAL_NEST
}
