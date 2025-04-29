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
#include <watchdog.hpp>

void 
setup(
    void
){
    // Debug messages to USB connection
    NEST_SERIAL_CONNECTION.begin(115200, SERIAL_8N1);

    // Serial connection to modem
    MODEM_SERIAL_CONNECTION.begin(9600, SERIAL_8N1);

    delay(100);

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Booting up...\r\n");
#endif // DEBUG_ON

    noInterrupts();

    motor_init();

    nmv3_init();

    oled_initialize();

    interrupts();

    registerAllServices();

    activitity_init();
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
        if (s->period == 0 || now - s->lastRun >= s->period) {
            s->busy     = false;
            s->fn();
            s->lastRun = now;
            anyBusy   |= s->busy;
        }
    }
  
    if (!anyBusy) {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Going to sleep...");
    #endif
    }
}
