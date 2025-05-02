#include "safe_arduino.hpp"
#include "device_state.hpp"
#include "get_set_macros.hpp"
#include "my_clock.hpp"
#include "services.hpp"
#include "motor.hpp"

extern Service motorServiceDesc;

enum WiggleState {
    WIGGLE_OFF,
    WIGGLE_OUT,
    WIGGLE_BACK,
    WIGGLE_HOME,

    NUM_WIGGLE_STATES
};

GET_SET_FUNC_DEF(int, motor_position, 0)
GET_SET_FUNC_DEF(int, motor_target, 0)
GET_SET_FUNC_DEF(bool, is_motor_running, false)
GET_SET_FUNC_DEF(int, wiggle_state, WIGGLE_OFF)
GET_SET_FUNC_DEF(int, wiggle_start_pos, 0)

volatile bool motorQuadratureEvent = false;

void
motor_quadrature_interrupt(
    void
){
    int delta = digitalRead(MOTOR_QUAD_B) ? +1 : -1;
    set_motor_position(get_motor_position() + delta);

    motorQuadratureEvent = true;
}

// Low‑level controls
void
motor_sleep(
    void
){
    digitalWrite(MOTOR_DRIVER_POWER, LOW);
}

void
motor_wake_up(
    void
){
    digitalWrite(MOTOR_DRIVER_POWER, HIGH);
}

void
motor_off(
    void
){
    digitalWrite(MOTOR_DRIVER_A, LOW);
    digitalWrite(MOTOR_DRIVER_B, LOW);
}

void
motor_forward(
    void
){
    digitalWrite(MOTOR_DRIVER_A, HIGH);
    digitalWrite(MOTOR_DRIVER_B, LOW);
}

void
motor_reverse(
    void
){
    digitalWrite(MOTOR_DRIVER_A, HIGH);
    digitalWrite(MOTOR_DRIVER_B, HIGH);
}

static int
wrap(
    int v
){
    int w = v % TICKS_PER_REV;

    if (w<0)
        w+=TICKS_PER_REV;

    return w;
}

static int
diff(
    int a,
    int b
){
    int d=(b-a)%TICKS_PER_REV;

    if(d<-TICKS_PER_REV/2)
        d+=TICKS_PER_REV;
    else if(d>TICKS_PER_REV/2)
        d-=TICKS_PER_REV;

    return d;
}

static int
wrappedPos(
    void
){
    return wrap(get_motor_position());
}

void
motor_run_to_position(
    int target
){
    set_motor_target(wrap(target));
}

// Schedule next wiggle by setting state and computing target
static void
scheduleWiggle(
    void
){
    set_wiggle_state(WIGGLE_OUT);
}

void
motorService(
    void
){
    motorServiceDesc.busy = false;

    static long settle = 0;

    int pos = wrappedPos();
    int target = get_motor_target();
    int remain = diff(pos, target);
    int wiggleState = get_wiggle_state();

#ifdef DEBUG_ON // DEBUG_ON
    static uint32_t lastPos = 0;

    Serial.printf("motorService: state=%d running=%d event=%d",
        wiggleState,
        get_is_motor_running(),
        motorQuadratureEvent);

    if (pos != lastPos) {
        Serial.printf(" pos=%d tgt=%d",
            pos,
            get_motor_target());

        lastPos = pos;
    }

    Serial.printf("\r\n");
#endif // DEBUG_ON

    if (pos != target || wiggleState != WIGGLE_OFF){
        switch(wiggleState){
            case WIGGLE_OUT:
                if(!get_is_motor_running()){
                    set_wiggle_start_pos(pos);
                    motor_run_to_position(get_wiggle_start_pos() + WIGGLE_OFFSET);
                }

                break;
            case WIGGLE_BACK:
                if(!get_is_motor_running()){
                    motor_run_to_position(get_wiggle_start_pos() + WIGGLE_OFFSET);
                }

                break;
            case WIGGLE_HOME:
                if(!get_is_motor_running()){
                    motor_run_to_position(get_wiggle_start_pos());
                }

                break;
            default:
                break;
        }

        motor_wake_up();

        if (remain < 0) motor_forward();
        else            motor_reverse();
        
        set_is_motor_running(true);

        motorServiceDesc.busy = true;
    }

    if (motorQuadratureEvent) {
        motorQuadratureEvent = false;

        if (abs(remain) <= MOTOR_DEADBAND && settle == 0) {
            motor_off();

            settle = InternalClock() + MOTOR_SETTLE_MS;
        }

        if (settle <= InternalClock()){
            set_is_motor_running(false);
            
            if (wiggle_state != WIGGLE_OFF){
                set_wiggle_state(wiggle_state % NUM_WIGGLE_STATES);
            }

            settle = 0;

            motor_sleep();
        }

        motorServiceDesc.busy = true;
    }
}

void
motor_init(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Motor Init...\r\n");
#endif  // DEBUG_ON

    pinMode(MOTOR_QUAD_A, INPUT);
    pinMode(MOTOR_QUAD_B, INPUT);

    attachInterrupt(MOTOR_QUAD_A, motor_quadrature_interrupt, RISING);

    pinMode(MOTOR_DRIVER_POWER, OUTPUT);
    pinMode(MOTOR_DRIVER_A, OUTPUT);
    pinMode(MOTOR_DRIVER_B, OUTPUT);

    motor_off();
    motor_sleep();

    Asr_Timer_Init();
    Asr_Timer_RegisterAlarmCallback(scheduleWiggle);
    Asr_SetTimeout(WIGGLE_INTERVAL_MS);
}
