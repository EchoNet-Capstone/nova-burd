#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"


void motor_sleep(void);
void motor_wake_up(void);
void motor_off(void);
void motor_reverse(void);
void motor_forward(void);
void motor_quadrature_interrupt(void);
void motor_run_to_position(int target);
void wiggle_motor(void);


#endif