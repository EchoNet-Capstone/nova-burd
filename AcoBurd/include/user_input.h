#ifndef USER_INPUT_H
#define USER_INPUT_H

// Interface GPIOs
#define INT_GPIO USER_KEY
#define REED_SWITCH_INPUT1 GPIO1
#define REED_SWITCH_INPUT_INT INT_GPIO        // Don't use this - causes tons of random interrupts
#define REED_SWITCH_INPUT2 GPIO2

#define REED_SWITCH_CALIBRATE 25
#define REED_SWITCH_SUPER_LONG_PRESS 60
#define REED_SWITCH_LONG_PRESS 15
#define REED_SWITCH_SHORT_PRESS 1

void user_input_init(void);
void interaction_service(void);
void reed_switch_debounce(void);

#endif