#include "safe_arduino.hpp"

#include "display.hpp"

#include "sleep.hpp"

bool wakeByUart = false;
volatile bool sleep_requested = false;

void
wakeUp(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.begin(115200);
    delay(100);

    Serial.print("Waking up....\r\n");
#endif // DEBUG_ON

#ifndef RECV_SERIAL_NEST // !RECV_SERIAL_NEST
    Serial1.begin(9600);
    delay(100);
#endif // !RECV_SERIAL_NEST

    VextON();
    delay(100);

    oled_wakeup();
}

void
goToSleep(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Going to sleep...\r\n");
#endif // DEBUG_ON

    oled_sleep();

    VextOFF();
    delay(100);


#ifdef DEBUG_ON // DEBUG_ON
    Serial.flush();
    Serial.end();

    delay(100);
#endif // DEBUG_ON

#ifndef RECV_SERIAL_NEST // !RECV_SERIAL_NEST
    Serial1.flush();
    Serial1.end();

    delay(100);
#endif // !RECV_SERIAL_NEST

    lowPowerHandler();

}