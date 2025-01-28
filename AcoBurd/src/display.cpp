#include <HT_SSD1306Wire.h>
#include <CubeCell_NeoPixel.h>

#include "display.h"
#include "my_clock.h"
#include "timers.h"
#include "device_state.h"
#include "sleep.h"
#include "battery.h"
#include "user_input.h"

SSD1306Wire oled(0x3c, 500000, SDA, SCL, GEOMETRY_128_64, GPIO10); // addr , freq , SDA, SCL, resolution , rst

// Configure RGB LED
CubeCell_NeoPixel rgbpixel(1, RGB, NEO_GRB + NEO_KHZ800);

// Sleep display for power saving
void oled_sleep(){
  oled.sleep();//OLED sleep
  //VextOFF();
}

// Wake up display
void oled_wake(){
  VextON();
  oled.wakeup();
}

void draw_days(int release_days){
  if(release_days < 10){
    oled.drawString(0, 40, "  " + (String)release_days);
    oled.drawString(1, 40, "  " + (String)release_days);
  }
  else if(release_days < 100){
    oled.drawString(0, 40, " " + (String)release_days);
    oled.drawString(1, 40, " " + (String)release_days);
  }
  else{
    oled.drawString(0, 40, (String)release_days);
    oled.drawString(1, 40, (String)release_days);
  }
}

void draw_hours(int release_hours){
  if(release_hours < 10){
    oled.drawString(42, 40, " " + (String)release_hours);
    oled.drawString(43, 40, " " + (String)release_hours);
  }
  else{
    oled.drawString(42, 40, (String)release_hours);
    oled.drawString(43, 40, (String)release_hours);
  }
}

void draw_minutes(int release_minutes){
  if(release_minutes < 10){
    oled.drawString(74, 40, "0" + (String)release_minutes);
    oled.drawString(75, 40, "0" + (String)release_minutes);
  }
  else{
    oled.drawString(74, 40, (String)release_minutes);
    oled.drawString(75, 40, (String)release_minutes);
  }
}

void draw_seconds(int release_seconds){
  if(release_seconds < 10){
    oled.drawString(107, 40, "0" + (String)release_seconds);
    oled.drawString(108, 40, "0" + (String)release_seconds);
  }
  else{
    oled.drawString(107, 40, (String)release_seconds);
    oled.drawString(108, 40, (String)release_seconds);
  }

}

void draw_main_view(int release_days, int release_hours, int release_minutes, int release_seconds){
  int reset_countdown = 0;

  if (get_reed_switch1() || get_reed_switch2()){
    reset_countdown = REED_SWITCH_LONG_PRESS - (InternalClock() - get_reed_switch_first_press());
  }
  else{
    reset_countdown = REED_SWITCH_LONG_PRESS;
  }


  oled.drawString(5, 15, "Hold magnet " + (String)reset_countdown + " seconds");
  oled.drawString(14, 24, "to reset timer to zero.");

  oled.setFont(ArialMT_Plain_16);

  draw_days(release_days);

  draw_hours(release_hours);

  draw_minutes(release_minutes);

  draw_seconds(release_seconds);

  // Reset font back to small
  oled.setFont(ArialMT_Plain_10);
  oled.drawString(0, 54, "DAYS    HRS    MIN     SEC");
}

void draw_secondary_view(){
  oled.drawString(8, 16, "Hold magnet for at least ");
  oled.drawString(12, 25, (String)REED_SWITCH_SHORT_PRESS + " second to set timer.");

  oled.setFont(ArialMT_Plain_16);
  oled.drawString(0, 45, "RELEASE OPEN");
  oled.drawString(1, 45, "RELEASE OPEN");
}

void draw_gps_string(){
  if(get_gps_lock() == 1){
    oled.drawString(85, 0, "GPS Lock");
  }
  else if(get_gps_enabled() == 1){
    oled.drawString(85, 0, "GPS On");
  }
  else{
    oled.drawString(85, 0, "GPS Off");
  }
}

void update_display(){
  // Time until release
  set_time_until_release(get_release_timer() - InternalClock());

  if(get_time_until_release() < 0){
    set_time_until_release(0);
  }

  int release_days = get_time_until_release() / 86400;
  int release_hours = (get_time_until_release() - (release_days * 86400) ) / 3600;
  int release_minutes = (get_time_until_release() - (release_days * 86400) - (release_hours * 3600) ) / 60;
  int release_seconds = (get_time_until_release() - (release_days * 86400) - (release_hours * 3600) - (release_minutes * 60) );

  // 9 pixel height works well for line spacing
  oled_wake();
  oled.clear();
  oled.setFont(ArialMT_Plain_10);
  oled.drawString(0, 0, "Battery: " + (String)get_battery_percent() + "%");

  draw_gps_string();

  if(get_time_until_release() > 0){
    draw_main_view(release_days, release_hours, release_minutes, release_seconds);
  }
  else{
    draw_secondary_view();
  }

  // Reset font back to small
  oled.setFont(ArialMT_Plain_10);

  oled.display();
}

void rgb_led(uint8_t red, uint8_t green, uint8_t blue){
  if((red > 0) || (green > 0) || (blue > 0)){
    //VextON();
    rgbpixel.begin(); // INITIALIZE RGB strip object (REQUIRED)
    rgbpixel.clear(); // Set all pixel colors to 'off'
    rgbpixel.setPixelColor(0, rgbpixel.Color(red, green, blue));
    // Send the updated pixel colors to the hardware.
    rgbpixel.show();
  }
  else{
    rgbpixel.begin(); // INITIALIZE RGB strip object (REQUIRED)
    rgbpixel.setPixelColor(0, rgbpixel.Color(red, green, blue));
    //rgbpixel.clear(); // Set all pixel colors to 'off'
    // Send the updated pixel colors to the hardware.
    rgbpixel.show();
  }
}

void led_flasher(){
  rgb_led(255, 0, 0);
  delay(20);
  rgb_led(0, 0, 0);
  delay(100);
  rgb_led(0, 255, 0);
  delay(20);
  rgb_led(0, 0, 0);

  //if (gps_lock == 1){
  //  rgb_led(0, 0, 32);
  //}
}

// Bootup company logo
void logo(){
  VextON();// oled power on;
  delay(10);

  oled.init();
  oled.clear();
  oled.display();

  oled.clear();
  oled.setFont(ArialMT_Plain_16);
  oled.drawString(10, 15, "NOVA");
  oled.drawString(11, 15, "NOVA");
  oled.drawString(12, 15, "NOVA");
  oled.drawString(35, 30, "ROBOTICS");
  oled.display();
  //oled.setFont(ArialMT_Plain_10);
}

// Waiting screen
void waiting_screen(){
  VextON();// oled power on;
  delay(10);

  oled.init();
  oled.clear();
  oled.display();

  oled.clear();
  oled.setFont(ArialMT_Plain_16);
  oled.drawString(25, 5, "WAITING...");
  //oled.setFont(ArialMT_Plain_10);
  oled.drawString(10, 35, "TAP MAGNET");
  oled.display();
  //oled.setFont(ArialMT_Plain_10);
}