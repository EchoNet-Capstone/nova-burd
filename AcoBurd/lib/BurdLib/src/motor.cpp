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

// wrap into [0 … TICKS_PER_REV-1]
int wrappedPos() {
    int w = get_motor_position() % TICKS_PER_REV;
    if (w < 0) w += TICKS_PER_REV;
    return w;
}


// wrap any target the same way
int wrapTarget(int t) {
    int w = t % TICKS_PER_REV;
    if (w < 0) w += TICKS_PER_REV;
    return w;
}

// minimal signed distance in [–halfRev … +halfRev]
int circularDiff(int from, int to) {
    int d = (to - from) % TICKS_PER_REV;
    if (d < -TICKS_PER_REV/2)   d += TICKS_PER_REV;
    else if (d >  TICKS_PER_REV/2) d -= TICKS_PER_REV;
    return d;
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

// Set motor target position
void
motor_run_to_position(
    int target
){
    set_motor_target(target);
    motor_wake_up();
}

static uint32_t stopTime = 0;
static bool wiggleInitialized[4] = { false, false, false, false };

void
motorService(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("\x1B[2J\x1B[H");

    Serial.printf(
        "SVC: pos=%d state=%d init=%d running=%d stopT=%lu now=%lu\n",
        wrappedPos(),
        get_wiggle_state(),
        wiggleInitialized[get_wiggle_state()] ? 1 : 0,
        get_is_motor_running() ? 1 : 0,
        stopTime,
        InternalClock()
      );
#endif

    motorServiceDesc.busy = false;

    if (get_is_motor_running() && stopTime != 0 && InternalClock() >= stopTime) {
        motor_off();
        set_is_motor_running(false);
        stopTime = 0;
        motorServiceDesc.busy = true;
    }

    // inside motorService(), before the switch:
    int s = get_wiggle_state();
    // if we’ve just entered a new state, clear its init flag
    static int lastState = -1;
    if (s != lastState) {
        wiggleInitialized[s] = false;
        lastState = s;
    }

    switch(get_wiggle_state()){
        case WIGGLE_1:

            if (!wiggleInitialized[WIGGLE_1]) {
                // first time in WIGGLE_1: set up and start
                set_wiggle_start_pos(wrappedPos());
                set_motor_target(wrapTarget(get_wiggle_start_pos() + 250));
                motor_wake_up();
                motor_forward();
                set_is_motor_running(true);
                motorServiceDesc.busy = true;
                wiggleInitialized[WIGGLE_1] = true;
            }
            else if (!get_is_motor_running()) {
                // once the motor has stopped, advance
                set_wiggle_state(WIGGLE_2);
            }
            break;
        case WIGGLE_2:

            if (!wiggleInitialized[WIGGLE_2]) {
                set_motor_target(wrapTarget(get_wiggle_start_pos() - 250));
                motor_wake_up();
                motor_reverse();
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
                set_motor_target(wrapTarget(get_wiggle_start_pos()));
                motor_wake_up();

                int pos = wrappedPos();           // 0…TICKS_PER_REV-1
                int tgt = get_motor_target();     // also wrapped
                int diff = circularDiff(pos, tgt);
                
                if (diff >  MOTOR_DEADBAND)      // target is ahead in the + direction
                    motor_forward();
                else if (diff < -MOTOR_DEADBAND) // target is behind
                    motor_reverse();
                else
                    motor_off();                  // we’re within dead-band, just coast

                set_is_motor_running(true);
                motorServiceDesc.busy = true;
                wiggleInitialized[WIGGLE_FINAL] = true;
            }
            else if (!get_is_motor_running()) {
                // wiggle is completely done — go back to sleep and re-arm
                set_wiggle_state(WIGGLE_OFF);
                Asr_SetTimeout(WIGGLE_INTERVAL_MS);
            }
            break;
        default:
            // ERROR: unhandled wiggle state
            break;
    }

  if (motorQuadratureEvent) {
    motorQuadratureEvent = false;

    int pos = wrappedPos();
    int tgt = get_motor_target();

    int diff = circularDiff(pos, tgt);
    // equality check is enough if you always hit exactly
    if (abs(diff) <= MOTOR_DEADBAND) {
      stopTime = InternalClock() + MOTOR_SETTLE_MS;
    }

    motorServiceDesc.busy = true;
  }

  // ensure busy if still running or wiggle is active
  if (get_is_motor_running() || get_wiggle_state() != WIGGLE_OFF) {
    motorServiceDesc.busy = true;
  }
}