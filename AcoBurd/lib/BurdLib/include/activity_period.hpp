#pragma once

#include "get_set_macros.hpp"

enum
activityState {
    LISTENING,
    SENDING,
    RANGING
};

GET_SET_FUNC_PROTO(activityState, activity_state)

void
activitity_init(
    void
);

void
activityService(
    void
);

bool
is_activity_period_open(
    void
);

