#include <LoRaWan_APP.h>

#include "safe_arduino.hpp"

void
goToSleep(

){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Going to sleep...\r\n");
#endif
    LoRaWAN.sleep();

    millis();
}