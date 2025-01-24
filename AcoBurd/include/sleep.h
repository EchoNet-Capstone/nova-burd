#ifndef SLEEP_H
#define SLEEP_H

void init_sleep(void);
void TimerWakeUp(void);
void go_to_sleep(void);
void gpio_interrupt(void);

bool is_low_power(void);

void set_reed_switch_first_press(long new_reed_switch_first_press);
long get_reed_switch_first_press(void);

void set_reed_switch_release_time(long new_reed_switch_release_time);
long get_reed_switch_release_time(void);

void set_timer_tap_multiplier1(int new_timer_tap_multiplier1);
int get_timer_tap_multiplier1(void);

void set_timer_tap_multiplier2(int new_timer_tap_multiplier2);
int get_timer_tap_multiplier2(void);

void set_wait_screen_delay(int new_wait_screen_delay);
int get_wait_screen_delay(void);

#endif