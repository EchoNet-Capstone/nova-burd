#ifndef MOTOR_H
#define MOTOR_H

// Motor and Encoder GPIO Setup
#define MOTOR_DRIVER_POWER GPIO7              // To motor driver Vcc AND motor encoder blue wire (encoder power)
#define MOTOR_DRIVER_A GPIO5                  // Motor driver ENABLE (Must be HIGH)
#define MOTOR_DRIVER_B GPIO6                  // Motor driver PHASE (Direction)
#define MOTOR_QUAD_A GPIO3                    // Motor encoder Yellow
#define MOTOR_QUAD_B GPIO4                    // Motor encoder White

// Motor and Gearbox Configuration
#define GEARBOX_RATIO 499
#define PULSES_PER_MOTOR_ROTATION 11
#define MOTOR_DEADBAND 50
#define WIGGLE_DEADBAND 259200                // If release will occur in next x seconds, then don't wiggle

// Silver AliExpress Motors - 2695 counts per rev
//#define CLOSED_POSITION (0.490 * GEARBOX_RATIO * PULSES_PER_MOTOR_ROTATION)                    
// Red Mark Pololu Motors
#define CLOSED_POSITION (0.550 * GEARBOX_RATIO * PULSES_PER_MOTOR_ROTATION)                    
#define OPEN_POSITION 0

void motor_init(void);
void set_motor_state(void);
void motor_sleep(void);
void motor_wake_up(void);
void motor_off(void);
void motor_reverse(void);
void motor_forward(void);
void motor_quadrature_interrupt(void);
void motor_run_to_position(int target);
void wiggle_motor(void);

// AHHHHH
void set_motor_target(int new_motor_target);
int get_motor_target(void);

void set_is_motor_running(bool new_is_motor_running);
bool get_is_motor_running(void);

#endif