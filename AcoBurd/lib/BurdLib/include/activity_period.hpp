#pragma once

#define NEIGHBOR_BLOCK 1
#define NEIGHBOR_UNBLOCK 0

enum
activityState {
    LISTENING,
    SENDING
};

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