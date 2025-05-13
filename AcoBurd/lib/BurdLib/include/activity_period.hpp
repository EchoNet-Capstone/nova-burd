#pragma once

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

