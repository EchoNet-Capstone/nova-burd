#include <timeServer.h>

// Global timer routine
long Corrected_time = 0;
TimerSysTime_t sysTimeCurrent;

long
InternalClock(
    void
){
    TimerSysTime_t sysTime = TimerGetSysTime();
    uint32_t millis = sysTime.Seconds * 1000 + ((uint32_t)sysTime.SubSeconds * 1000) / 65536;
    return (long)millis;
}