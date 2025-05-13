#include "safe_arduino.hpp"

#include "display.hpp"

void
wakeUp(
    void
){
    VextON();
    delay(100);

    oled_wakeup();
}

void
goToSleep(
    void
){
    oled_sleep();

    VextOFF();

    lowPowerHandler();
}