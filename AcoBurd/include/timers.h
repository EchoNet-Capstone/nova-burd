#ifndef TIMERS_H
#define TIMERS_H

// Timer Add Configuration
#define release_timer_add_1 7200              // Time to add for each magnet tap
#define release_timer_add_2 86400             // Time to add for each magnet tap
#define release_timer_first_press_1 120        // First tap starts clock at this number
#define release_timer_first_press_2 172830      // First tap starts clock at this number (add an extra 30 seconds for readability)
#define display_timeout 20                    // Must be bigger than reset timer (15s)
#define encoder_timeout 10

long get_release_timer(void);
void set_release_timer(long new_release_timer);
long get_time_until_release(void);
void set_time_until_release(long new_time_until_release);

long get_battery_timer(void); // no use - remove later
void set_battery_timer(long new_battery_timer);

long get_encoder_timer(void);
void set_encoder_timer(long new_encoder);

long get_display_timer();
void set_display_timer(long new_display);

void set_wiggle_timer(long new_wiggle_timer);
long get_wiggle_timer(void);

#endif
