#ifndef ACTIVITY_PERIOD_H
#define ACTIVITY_PERIOD_H

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
activity_update(
    void
);

enum activityState
is_activity_period_open(
    void
);

#endif