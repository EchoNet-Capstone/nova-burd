#include "safe_arduino.hpp"

#include "device_state.hpp"
#include "get_set_macros.hpp"
#include "my_clock.hpp"
#include "services.hpp"

#include "motor.hpp"

extern Service motorServiceDesc;

enum wiggleStates {
    WIGGLE_OFF,
    WIGGLE_1,
    WIGGLE_2,
    WIGGLE_FINAL,
};

GET_SET_FUNC_DEF(int, motor_position, 0)
GET_SET_FUNC_DEF(int, motor_target, 0)

GET_SET_FUNC_DEF(bool, is_motor_running, false)

GET_SET_FUNC_DEF(int, wiggle_state, WIGGLE_OFF)
GET_SET_FUNC_DEF(int, wiggle_start_pos, 0)

volatile bool motorQuadratureEvent = false;

void motor_quadrature_interrupt() {
    if (digitalRead(MOTOR_QUAD_B)) {
        set_motor_position(get_motor_position() + 1);
    } else {
        set_motor_position(get_motor_position() - 1);
    }
    motorQuadratureEvent = true;     // wake up the service
}

// Wiggle the motor to clear barnacles
void
wiggle_motor(
    void
){
    set_wiggle_state(WIGGLE_1);
}

void
motor_init(
    void
){
    // Setup motor quadrature and interrupt
    pinMode(MOTOR_QUAD_A, INPUT);
    pinMode(MOTOR_QUAD_B, INPUT);
    attachInterrupt(MOTOR_QUAD_A, motor_quadrature_interrupt, RISING);

    // Setup motor driver - Uses 1-2 mA
    pinMode(MOTOR_DRIVER_POWER, OUTPUT);
    pinMode(MOTOR_DRIVER_A, OUTPUT);
    pinMode(MOTOR_DRIVER_B, OUTPUT);
    digitalWrite(MOTOR_DRIVER_POWER, LOW);
    digitalWrite(MOTOR_DRIVER_A, LOW);
    digitalWrite(MOTOR_DRIVER_B, LOW);

    Asr_Timer_Init();
    Asr_Timer_RegisterAlarmCallback(wiggle_motor);
    Asr_SetTimeout(WIGGLE_INTERVAL_MS);
}

// Motor Power Save
void
motor_sleep(
    void
){
    digitalWrite(MOTOR_DRIVER_POWER, LOW);
}

// Motor Wake Up
void
motor_wake_up(
    void
){
    digitalWrite(MOTOR_DRIVER_POWER, HIGH);
}

// Turn Motor Off
void
motor_off(
    void
){
    digitalWrite(MOTOR_DRIVER_A, LOW);
    digitalWrite(MOTOR_DRIVER_B, LOW);
}

// Run Motor Reverse
void
motor_reverse(
    void
){
    digitalWrite(MOTOR_DRIVER_A, HIGH);
    digitalWrite(MOTOR_DRIVER_B, LOW);
}

// Run Motor Forward
void
motor_forward(
    void
){
    digitalWrite(MOTOR_DRIVER_A, HIGH);
    digitalWrite(MOTOR_DRIVER_B, HIGH);
}

// Set motor target position
void
motor_run_to_position(
    int target
){
    set_motor_target(target);
    motor_wake_up();
}

void
motorService(
    void
){
    motorServiceDesc.busy = false;

    switch(get_wiggle_state()){
        case WIGGLE_OFF:
            break;
        case WIGGLE_1:
            set_wiggle_start_pos(get_motor_position());
            set_motor_target(get_wiggle_start_pos() + 250);

            Asr_Timer_Disable();                     // clear any old timeout

            motorServiceDesc.busy = true;

            if(!get_is_motor_running()){
                set_wiggle_state(WIGGLE_2);
            }
            break;
        case WIGGLE_2:
            set_motor_target(get_wiggle_start_pos() - 250);
            
            motorServiceDesc.busy = true;

            if(!get_is_motor_running()){
                set_wiggle_state(WIGGLE_FINAL);
            }
            break;
        case WIGGLE_FINAL:
            set_motor_target(get_wiggle_start_pos());
            
            motorServiceDesc.busy = true;

            if(!get_is_motor_running()){
                set_wiggle_state(WIGGLE_OFF);

                Asr_SetTimeout(WIGGLE_INTERVAL_MS);
            }
            break;
        default:
            // ERROR: unhandled wiggle state
            break;
    }

    // If we just set a new target, immediately start the motor
    if (motorServiceDesc.busy == true && !get_is_motor_running()) {
        motor_wake_up();
        if (get_motor_position() < get_motor_target()) {
            motor_forward();
        } else {
            motor_reverse();
        }
        set_is_motor_running(true);
    }

    if (motorQuadratureEvent) {
        motorQuadratureEvent = false;

        int pos = get_motor_position();
        int tgt = get_motor_target();
        
        // we’re in range, but wait a few ms for inertia to die out
        static uint32_t stopTime = 0;
        if (stopTime == 0) {
            stopTime = InternalClock() + MOTOR_SETTLE_MS;
        }
        else if (InternalClock() >= stopTime) {
            motor_off();
            set_is_motor_running(false);
            stopTime = 0;
            motorServiceDesc.busy = true;
        }
    }

    if (get_is_motor_running()){
        motorServiceDesc.busy = true;
    }
}