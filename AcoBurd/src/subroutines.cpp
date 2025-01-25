#include <Arduino.h>
#include <stdio.h>

#include "subroutines.h"
#include "my_clock.h"
#include "device_state.h"
#include "timers.h"
#include "globals.h"
#include "motor.h"

void debug_subroutine(void){
  Serial.printf("Main Clock: %ld Time Until Release: %ld Encoder Time: %ld Encoder Power: %d Vext: %d\n"
  , InternalClock(), get_time_until_release(), get_encoder_timer(), digitalRead(MOTOR_DRIVER_POWER), digitalRead(Vext));
}

