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

#define TICKS_PER_REV 2695

void motor_quadrature_interrupt() {
    int delta = digitalRead(MOTOR_QUAD_B) ? +1 : -1;
    int raw = get_motor_position() + delta;
    set_motor_position(raw);
    motorQuadratureEvent = true;
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
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Motor Init...\r\n");
#endif // DEBUG_ON

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
    digitalWrite(MOTOR_DRIVER_B, HIGH);
}

// Run Motor Forward
void
motor_forward(
    void
){
    digitalWrite(MOTOR_DRIVER_A, HIGH);
    digitalWrite(MOTOR_DRIVER_B, LOW);
}

static uint32_t    stopTime             = 0;
static bool        wiggleInitialized[4] = {false,false,false,false};
static int         lastState            = -1;

// wrap any raw value into [0 .. TICKS_PER_REV-1]
static int wrapTarget(int raw) {
    int w = raw % TICKS_PER_REV;
    if (w < 0) w += TICKS_PER_REV;
    return w;
}

// signed shortest-path diff in [−TICKS_PER_REV/2 .. +TICKS_PER_REV/2]
static int circularDiff(int from, int to) {
    int d = (to - from) % TICKS_PER_REV;
    if (d < -TICKS_PER_REV/2)    d += TICKS_PER_REV;
    else if (d >  TICKS_PER_REV/2) d -= TICKS_PER_REV;
    return d;
}

// get the current wrapped position
static int wrappedPos() {
    return wrapTarget(get_motor_position());
}

// Set motor target position
void
motor_run_to_position(
    int target
){
    set_motor_target(wrapTarget(target));
    motor_wake_up();
}


void
motorService(
    void
){
    motorServiceDesc.busy = false;
    int s = get_wiggle_state();

    // clear "init" flag on state change
    if (s != lastState) {
        wiggleInitialized[s] = false;
        lastState = s;
    }

    // 1) Stop‐by‐timeout logic
    if (get_is_motor_running() && stopTime != 0 && InternalClock() >= stopTime) {
        motor_off();
        set_is_motor_running(false);
        stopTime = 0;
        motorServiceDesc.busy = true;
    }

    // 2) Wiggle state‐machine
    switch (s) {
      case WIGGLE_OFF:
        if (!wiggleInitialized[WIGGLE_OFF]) {
            motor_off();
            set_is_motor_running(false);
            wiggleInitialized[WIGGLE_OFF] = true;
        }
        break;

      case WIGGLE_1:
        if (!wiggleInitialized[WIGGLE_1]) {
            int start = wrappedPos();
            set_wiggle_start_pos(start);
            set_motor_target(wrapTarget(start + WIGGLE_OFFSET));

            motor_wake_up();
            motor_forward();              // enable=A=HIGH, phase=B=LOW
            set_is_motor_running(true);

            motorServiceDesc.busy = true;
            wiggleInitialized[WIGGLE_1] = true;
        }
        else if (!get_is_motor_running()) {
            set_wiggle_state(WIGGLE_2);
        }
        break;

      case WIGGLE_2:
        if (!wiggleInitialized[WIGGLE_2]) {
            int start = get_wiggle_start_pos();
            set_motor_target(wrapTarget(start - WIGGLE_OFFSET));

            motor_wake_up();
            motor_reverse();              // enable=A=HIGH, phase=B=HIGH
            set_is_motor_running(true);

            motorServiceDesc.busy = true;
            wiggleInitialized[WIGGLE_2] = true;
        }
        else if (!get_is_motor_running()) {
            set_wiggle_state(WIGGLE_FINAL);
        }
        break;

      case WIGGLE_FINAL:
        if (!wiggleInitialized[WIGGLE_FINAL]) {
            int start = get_wiggle_start_pos();
            int tgt   = wrapTarget(start);
            set_motor_target(tgt);

            motor_wake_up();
            // choose shortest arc
            if (circularDiff(wrappedPos(), tgt) > 0) motor_forward();
            else                                     motor_reverse();
            set_is_motor_running(true);

            motorServiceDesc.busy = true;
            wiggleInitialized[WIGGLE_FINAL] = true;
        }
        else if (!get_is_motor_running()) {
            set_wiggle_state(WIGGLE_OFF);
            Asr_SetTimeout(WIGGLE_INTERVAL_MS);
        }
        break;
    }

    // 3) On each encoder tick, arm the settle timer when within dead-band
    if (motorQuadratureEvent) {
        motorQuadratureEvent = false;

        int pos  = wrappedPos();
        int tgt  = get_motor_target();
        int diff = circularDiff(pos, tgt);

        if (abs(diff) <= MOTOR_DEADBAND) {
            stopTime = InternalClock() + MOTOR_SETTLE_MS;
        }
        motorServiceDesc.busy = true;
    }

    // 4) Stay busy until wiggle is fully complete
    if (get_is_motor_running() || s != WIGGLE_OFF) {
        motorServiceDesc.busy = true;
    }
}