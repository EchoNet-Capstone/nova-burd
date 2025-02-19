#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <Arduino.h>

void oled_sleep(void);
void oled_wake(void);
void oled_initialize(void);
void display_modem_id(int modem_id);

#endif