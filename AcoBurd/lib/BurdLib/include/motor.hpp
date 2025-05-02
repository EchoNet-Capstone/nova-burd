#pragma once

// Motor and Encoder GPIO Setup
#define MOTOR_DRIVER_POWER GPIO7              // To motor driver Vcc AND motor encoder blue wire (encoder power)
#define MOTOR_DRIVER_A GPIO5                  // Motor driver ENABLE (Must be HIGH)
#define MOTOR_DRIVER_B GPIO6                  // Motor driver PHASE (Direction)
#define MOTOR_QUAD_A GPIO3                    // Motor encoder Yellow
#define MOTOR_QUAD_B GPIO4                    // Motor encoder White

// Motor and Gearbox Configuration
#define GEARBOX_RATIO 499
#define PULSES_PER_MOTOR_ROTATION 12 // Single Quadtrature
#define MOTOR_DEADBAND 3
#define MOTOR_SETTLE_MS 50
#define WIGGLE_DEADBAND 259200                // If release will occur in next x seconds, then don't wiggle
#define WIGGLE_INTERVAL_MS 10000
#define WIGGLE_OFFSET 250

#define TICKS_PER_REV    499 * 12

void
motor_quadrature_interrupt(
    void
);

void
motor_init(
    void
);

void
motorService(
    void
);

void
motor_sleep(
    void
);

void
motor_run_to_position(
    int target
);