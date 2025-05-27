#include <timeServer.h>

// Global timer routine
TimerSysTime_t sysTimeCurrent;

uint32_t
InternalClock(
    void
){
    TimerSysTime_t sysTime = TimerGetSysTime();
    uint32_t millis = sysTime.Seconds * 1000 + ((uint32_t)sysTime.SubSeconds * 1000) / 65536;
    return millis;
}