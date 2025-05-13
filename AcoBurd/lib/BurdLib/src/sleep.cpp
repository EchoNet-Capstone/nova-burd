#include "safe_arduino.hpp"

#include "display.hpp"

void
goToSleep(

){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Going to sleep...\r\n");
    delay(1000);
#endif

    oled_sleep();

    VextOFF();

    lowPowerHandler();

#ifdef DEBUG_ON // DEBUG_ON
    Serial.print("Waking up....\r\n");
#endif

    VextON();
    delay(100);

    oled_wakeup();
}