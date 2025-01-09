#ifndef DISPLAY_H
#define DISPLAY_H

#include <main.h>

void oled_sleep(void);
void oled_wake(void);
void update_display(void);
void rgb_led(uint8_t red, uint8_t green, uint8_t blue);
void led_flasher();
void logo();
void waiting_screen();

#endif