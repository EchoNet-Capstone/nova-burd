#include "timers.h"

// timer vars
long display_timer;
long encoder_timer;
long release_timer;
long release_timer1;
long release_timer2;
long battery_timer;
long time_until_release;
long wiggle_timer;

long get_encoder_timer(){
  return encoder_timer;
}

void set_encoder_timer(long new_encoder){
  encoder_timer = new_encoder;
}

long get_release_timer(){
  return release_timer;
}

void set_release_timer(long new_release_timer){
  release_timer = new_release_timer;
}

long get_time_until_release(){
  return time_until_release;
}

void set_time_until_release(long new_time_until_release){
  time_until_release = new_time_until_release;
}

long get_battery_timer(){
  return battery_timer;
}

void set_battery_timer(long new_battery_timer){
   battery_timer = new_battery_timer;
}

long get_display_timer(){
  return display_timer;
}

void set_display_timer(long new_display){
  display_timer = new_display;
}

void set_wiggle_timer(long new_wiggle_timer){
  wiggle_timer = new_wiggle_timer; 
}

long get_wiggle_timer(){
  return wiggle_timer;
}