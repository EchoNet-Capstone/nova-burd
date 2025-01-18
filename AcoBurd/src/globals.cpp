#include "globals.hpp"
#include "GPS_Air530Z.h"

// Global timer routine
 long Corrected_time = 0;
 TimerSysTime_t sysTimeCurrent;

long InternalClock() {

  sysTimeCurrent = TimerGetSysTime( );
  Corrected_time = (long)sysTimeCurrent.Seconds;

  // How to set the clock
  //TimerSysTime_t newSysTime ;         // Make a new variable of type TimerSysTime_t (.Seconds and .SubSeconds)
  //newSysTime.Seconds = 1000;          // Store 1000 in .Seconds
  //newSysTime.SubSeconds = 50;         // Store 50 in .SubSeconds
  //TimerSetSysTime( newSysTime );      // Update time from variable newSysTime

  return Corrected_time;
}
