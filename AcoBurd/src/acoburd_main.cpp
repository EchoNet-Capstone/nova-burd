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
#include <neighbor.hpp>
#include <bloomfilter.hpp>

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

    EEPROM_init();

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

    // neighbor init
    neighborManager.clear_neighbors();

    bloom_reset();
}

extern volatile bool sleep_requested;

void 
loop(
    void
){
    static uint32_t activityTimeout = 0;
    static bool in_sleep = false;

    if (sleep_requested && in_sleep)
    { // waiting for MCU to actually go to sleep
        return;
    }
    else if (sleep_requested && !in_sleep)
    { // we've asked to sleep, but we haven't told the device yet
        in_sleep = true;
        goToSleep();

        return;
    }
    else if (!sleep_requested && in_sleep)
    { // we've woken up from sleep, execute wakeup
        wakeUp();

        activityTimeout = 0;
        in_sleep = false;
        return;
    }
    else
    {
        // Normal Operation
    }

#ifdef DEBUG_ON // DEBUG_ON
    static bool print_start_loop = true;

    if (print_start_loop)
    {
        Serial.printf("Initialization Completed. Starting Services...\r\n");

        print_start_loop = false;
    }
#endif // DEBUG_ON

    auto &svcs = ServiceRegistry::instance().services();
    uint32_t now = millis();
    bool anyBusy = false;

    for (auto *s : svcs)
    {
        if (s->period == 0 || (now - s->lastRun) >= s->period)
        {
            s->busy = false;
            s->fn();

            anyBusy |= s->busy;

            if (s->busy)
            {
                s->lastRun = now;
            }
        }
        else
        {
            /* Do nothing */
        }
    }

#ifndef RECV_SERIAL_NEST // !RECV_SERIAL_NEST
    if (!anyBusy)
    {
        if (activityTimeout == 0)
            activityTimeout = now + 100;

        if (activityTimeout != 0 && (int32_t)(now - activityTimeout) >= 0)
        {
            sleep_requested = true;
        }
    }
    else
    {
        activityTimeout = 0;
    }
#endif // !RECV_SERIAL_NEST
}
