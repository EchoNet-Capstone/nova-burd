#pragma once

#include <stdint.h>

#include "get_set_macros.hpp"

typedef enum
motorStatus_e {
    STOPPED,
    RUNNING,
    WIGGLING,

    NUM_MOTOR_STATUS
};

GET_SET_FUNC_PROTO(uint8_t, motor_status)

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
newMotorTarget(
    int target
);