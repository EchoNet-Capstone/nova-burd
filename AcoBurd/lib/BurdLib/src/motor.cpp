#include "safe_arduino.hpp"

#include "device_state.hpp"
#include "get_set_macros.hpp"
#include "my_clock.hpp"
#include "services.hpp"

#include "motor.hpp"

// Motor and Encoder GPIO Setup
#define MOTOR_DRIVER_POWER GPIO7              // To motor driver Vcc AND motor encoder blue wire (encoder power)
#define MOTOR_DRIVER_A GPIO5                  // Motor driver ENABLE (Must be HIGH)
#define MOTOR_DRIVER_B GPIO6                  // Motor driver PHASE (Direction)
#define MOTOR_QUAD_A GPIO3                    // Motor encoder Yellow
#define MOTOR_QUAD_B GPIO4                    // Motor encoder White

// Motor and Gearbox Configuration
#define GEARBOX_RATIO 499
#define PULSES_PER_MOTOR_ROTATION 48          // Full quadrature (12 pulses × 4)
#define MOTOR_DEADBAND 3
#define MOTOR_SETTLE_MS 50
#define WIGGLE_DEADBAND 259200                // If release will occur in next x seconds, then don't wiggle
#define MILLISECONDS 1000
#define WIGGLE_OFFSET 4000

#ifndef WIGGLE_INTERVAL_SECONDS
    //                              min *   hour    *   day *   num_days
    #define WIGGLE_INTERVAL_SECONDS ( 60  *   60      *   24  *   3 )
#endif
#define WIGGLE_INTERVAL_MS (WIGGLE_INTERVAL_SECONDS * MILLISECONDS)

#define TICKS_PER_REV    (GEARBOX_RATIO * PULSES_PER_MOTOR_ROTATION)

extern Service motorServiceDesc;

enum WiggleState {
    WIGGLE_OFF,
    WIGGLE_OUT,
    WIGGLE_BACK,
    WIGGLE_HOME_PREP,
    WIGGLE_HOME,

    NUM_WIGGLE_STATES
};

GET_SET_FUNC_DEF(uint8_t, motor_status, STOPPED)
GET_SET_FUNC_DEF(int, motor_position, 0)
GET_SET_FUNC_DEF(int, motor_target, 0)
GET_SET_FUNC_DEF(bool, is_motor_running, false)
GET_SET_FUNC_DEF(int, wiggle_state, WIGGLE_OFF)
GET_SET_FUNC_DEF(int, wiggle_start_pos, 0)

static int original_home_pos = 0;
static bool home_initialized = false;

// Add these global variables
volatile uint8_t encoder_last_state = 0;

// Add this common encoder processing function
void process_encoder_change(void) {
    // Read current state of both encoder channels
    uint8_t current_state = (digitalRead(MOTOR_QUAD_A) << 1) | digitalRead(MOTOR_QUAD_B);
    
    // Lookup table for the state transitions
    // Each row represents previous state (0-3)
    // Each column represents current state (0-3)
    // Values: 0 = invalid/no change, 1 = clockwise, -1 = counter-clockwise
    static const int8_t state_table[4][4] = {
        {0, 1, -1, 0},  // Changed signs compared to original
        {-1, 0, 0, 1},
        {1, 0, 0, -1},
        {0, -1, 1, 0}
    };
    
    // Get direction from the lookup table
    int8_t delta = state_table[encoder_last_state][current_state];
    
    // Update position
    if (delta != 0) {
        set_motor_position(get_motor_position() + delta);
    }
    
    // Save current state for next time
    encoder_last_state = current_state;
}

// Replace the existing interrupt handler with these two functions
void 
motor_quad_a_interrupt(
    void
){
    process_encoder_change();
}

void
motor_quad_b_interrupt(
    void
){
    process_encoder_change();
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

int
wrap(
    int v
){
    // If we're setting a special target (like home position), 
    // we want to use absolute positioning
    if (v == original_home_pos) {
        return v; // Don't wrap the home position
    }
    
    // Otherwise use modular arithmetic
    int w = v % TICKS_PER_REV;
    
    if (w < 0)
        w += TICKS_PER_REV;
        
    return w;
}

int
diff(
    int a,
    int b
){
    // If target is home position, use shortest path
    if (b == original_home_pos) {
        // Calculate both possible paths
        int direct_diff = b - a;
        int wrapped_diff;
        
        if (direct_diff > 0) {
            wrapped_diff = direct_diff - TICKS_PER_REV;
        } else {
            wrapped_diff = direct_diff + TICKS_PER_REV;
        }
        
        // Return the smallest absolute difference
        return (abs(direct_diff) < abs(wrapped_diff)) ? direct_diff : wrapped_diff;
    }
    
    // Regular modular arithmetic for other positions
    int d = (b-a) % TICKS_PER_REV;
    
    if(d < -TICKS_PER_REV/2)
        d += TICKS_PER_REV;
    else if(d > TICKS_PER_REV/2)
        d -= TICKS_PER_REV;
        
    return d;
}

int
wrappedPos(
    void
){
    return wrap(get_motor_position());
}

void 
driveToward(
    int remain
){
    motor_wake_up();
    if (remain > 0)  motor_forward();
    else             motor_reverse();
    set_is_motor_running(true);
    motorServiceDesc.busy = true;
}

void
newMotorTarget(
    int target
){
    set_motor_target(wrap(target));
}

// Schedule next wiggle by setting state and computing target
extern volatile bool sleep_requested;

void
scheduleWiggle(
    void
){
    Asr_Timer_Disable();

    set_wiggle_state(WIGGLE_OUT);

    if(!home_initialized){
        original_home_pos = wrappedPos();
        home_initialized = true;
    }

    set_wiggle_start_pos(original_home_pos);

    sleep_requested = false;
}

void
debugMotorService(
    int pos,
    int target,
    int remain,
    long settle
){
    static int lastPos = 0;
    static int lastTarget = 0;
    static int lastState = 0;
    static int lastRemain = 0;
    static uint32_t lastSettle = 0;
    static bool lastRunning = 0;

    if( get_wiggle_state() != lastState || lastRunning != get_is_motor_running()){
        Serial.printf("motorService: time:%ld state=%d running=%d",
            InternalClock(),
            get_wiggle_state(),
            get_is_motor_running());
    
        if (pos != lastPos || target != lastTarget || remain != lastRemain) {
            Serial.printf(" pos=%d tgt=%d remain=%d",
                pos,
                get_motor_target(),
                remain);
        }

        if(settle != lastSettle ){
            Serial.printf(" settle=%ld",
                settle);
        }

        Serial.printf("\r\n");
    }
    
    lastPos = pos;
    lastTarget = target;
    lastState = get_wiggle_state();
    lastSettle = settle;
    lastRemain = remain;
    lastRunning = get_is_motor_running();
}

void
wiggleState(
    void
){
    int wiggleState = get_wiggle_state();

    switch(wiggleState){
        case WIGGLE_OUT:
            if(!get_is_motor_running()){
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Starting wiggle out at pos=%d, home=%d\r\n", 
                    wrappedPos(), 
                    get_wiggle_start_pos());
            #endif

                newMotorTarget(get_wiggle_start_pos() + WIGGLE_OFFSET);
            }

            break;
        case WIGGLE_BACK:
            if(!get_is_motor_running()){
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Starting wiggle back at pos=%d, home=%d\r\n", 
                    wrappedPos(), 
                    get_wiggle_start_pos());
            #endif 

                newMotorTarget(get_wiggle_start_pos() - WIGGLE_OFFSET);
            }

            break;
        case WIGGLE_HOME_PREP:
            if(!get_is_motor_running()){
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Preparing for home approach at pos=%d, home=%d\r\n", 
                    wrappedPos(), 
                    get_wiggle_start_pos());
            #endif

                newMotorTarget(get_wiggle_start_pos() - 100);
            }

            break;
        case WIGGLE_HOME:
            if(!get_is_motor_running()){
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Starting final approach at pos=%d, home=%d\r\n", 
                    wrappedPos(), 
                    get_wiggle_start_pos());
            #endif
                
                newMotorTarget(get_wiggle_start_pos());
            }

            break;
        default:
            break;
    }
}

void
motorService(
    void
){
    motorServiceDesc.busy = false;

    static uint32_t settle = 0;

    if(get_wiggle_state() != WIGGLE_OFF){
        set_motor_status(WIGGLING);

        wiggleState();

        motorServiceDesc.busy = true;
    }

    int pos = wrappedPos();
    int target = get_motor_target();
    int remain = diff(pos, target);

// #ifdef DEBUG_ON // DEBUG_ON
//     debugMotorService(
//         pos,
//         target,
//         remain,
//         settle
//     );
// #endif // DEBUG_ON

    if (pos != target && settle == 0) {
        if (get_wiggle_state() == WIGGLE_OFF){
            set_motor_status(RUNNING);
        }

        driveToward(remain);

        motorServiceDesc.busy = true;
    }

    if (get_is_motor_running() && settle == 0) {
        if(abs(remain) <= MOTOR_DEADBAND){
            motor_off();

            settle = InternalClock() + MOTOR_SETTLE_MS;
        }

        motorServiceDesc.busy = true;
    }

    if (settle != 0 && (int32_t)(InternalClock() - settle) >= 0){      
        set_is_motor_running(false);

        switch(get_wiggle_state()){
            case WIGGLE_OUT:
                set_wiggle_state(WIGGLE_BACK);

                break;
            case WIGGLE_BACK:
                set_wiggle_state(WIGGLE_HOME_PREP);

                break;
            case WIGGLE_HOME_PREP:
                set_wiggle_state(WIGGLE_HOME);

                break;
            case WIGGLE_HOME:
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Finished Wiggle :)\r\n");
            #endif 

                set_wiggle_state(WIGGLE_OFF);
                set_motor_target(get_wiggle_start_pos());

                Asr_SetTimeout(WIGGLE_INTERVAL_MS);

                break;
            default:
                break;
        }

        set_motor_status(STOPPED);

        settle = 0;

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

    original_home_pos = 0;
    home_initialized = false;

    pinMode(MOTOR_QUAD_A, INPUT);
    pinMode(MOTOR_QUAD_B, INPUT);
    
    // Attach interrupts to BOTH channels, detecting BOTH edges
    attachInterrupt(MOTOR_QUAD_A, motor_quad_a_interrupt, CHANGE);
    attachInterrupt(MOTOR_QUAD_B, motor_quad_b_interrupt, CHANGE);

    pinMode(MOTOR_DRIVER_POWER, OUTPUT);
    pinMode(MOTOR_DRIVER_A, OUTPUT);
    pinMode(MOTOR_DRIVER_B, OUTPUT);

    set_motor_status(STOPPED);

    motor_off();
    motor_sleep();

    Asr_Timer_Init();
    Asr_Timer_RegisterAlarmCallback(scheduleWiggle);
    Asr_SetTimeout(WIGGLE_INTERVAL_MS);
}
