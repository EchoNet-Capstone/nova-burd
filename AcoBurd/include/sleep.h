#ifndef SLEEP_H
#define SLEEP_H

// Interrupt Timer Configuration
#define TIMETILLWAKEUP 1000                   // Cannot be longer than 1.4 seconds due to WDT

void init_sleep(void);
void TimerWakeUp(void);
void go_to_sleep(void);
void gpio_interrupt(void);

#endif