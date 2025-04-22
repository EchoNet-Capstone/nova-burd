#pragma once

enum activityState {
    LISTENING,
    SENDING
};


void activitity_init();
void activity_update();
enum activityState is_activity_period_open();