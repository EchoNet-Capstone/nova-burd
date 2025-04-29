#include <timeServer.h>

// Global timer routine
long Corrected_time = 0;
TimerSysTime_t sysTimeCurrent;

long
InternalClock(
    void
){
    sysTimeCurrent = TimerGetSysTime();
    Corrected_time = (long)sysTimeCurrent.Seconds;

    // How to set the clock
    // Make a new variable of type TimerSysTime_t (.Seconds and .SubSeconds)
    //TimerSysTime_t newSysTime ;
    // Store 1000 in .Seconds
    //newSysTime.Seconds = 1000;
    // Store 50 in .SubSeconds
    //newSysTime.SubSeconds = 50;
    // Update time from variable newSysTime
    //TimerSetSysTime( newSysTime );

    return Corrected_time;
}