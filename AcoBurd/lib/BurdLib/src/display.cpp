#include <CubeCell_NeoPixel.h>
#include <HT_SSD1306Wire.h>

#include "safe_arduino.hpp"

#include <stdio.h>

#include <nmv3_api.hpp>

#include "display.hpp"
#include "services.hpp"

#define NR_diagonal_width 16
#define NR_diagonal_height 16
static const uint8_t NR_diagonal_bits[] = {
   0x81,0x00,  /* row 0:    N:  1000_0001....... */
   0x83,0x00,  /* row 1:    N:  1000_0011....... */
   0x85,0x00,  /* row 2:    N:  1000_0101....... */
   0x89,0x00,  /* row 3:    N:  1000_1001....... */
   0x91,0x00,  /* row 4:    N:  1001_0001....... */
   0xA1,0x00,  /* row 5:    N:  1010_0001....... */
   0xC1,0x00,  /* row 6:    N:  1100_0001....... */
   0x81,0x00,  /* row 7:    N:  1000_0001....... */
   0x00,0x0F,  /* row 8:    R:  ........00001111 (R top bar) */
   0x00,0x09,  /* row 9:    R:  ........00001001 */
   0x00,0x09,  /* row 10:   R:  ........00001001 */
   0x00,0x0F,  /* row 11:   R:  ........00001111 (R bowl bottom) */
   0x00,0x05,  /* row 12:   R:  ........00000101 (R leg diag) */
   0x00,0x09,  /* row 13:   R:  ........00001001 */
   0x00,0x11,  /* row 14:   R:  ........00010001 */
   0x00,0x21   /* row 15:   R:  ........00100001 */
};

SSD1306Wire oled(0x3c, 500000, SDA, SCL, GEOMETRY_128_64, GPIO10); // addr , freq , SDA, SCL, resolution , rst

void
VextON(
    void
){
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);
}

void
VextOFF(
    void
){
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, HIGH);
}

void
oled_sleep(
    void
){
    oled.sleep();
}

// Wake up display
void
oled_wake(
    void
){
    VextON();
    oled.wakeup();
}

void
display_modem_id(
    void
){
    if(get_modem_id_set()){
        oled.drawString(0, 0, "  Modem ID : " + String(get_modem_id(), DEC));
    }else{
        oled.drawString(0, 0, "  Modem ID : UNKNOWN");
    }

    oled.display();
}

void
display_devicd_id(
    void
){

}

/*void
display_modem_packet_data(
    String packetBuffer
){
        oled.drawString(0, 20, "  Packet : " + packetBuffer);
        oled.display();
}*/

void
display_modem_packet_data(
    uint8_t *packetBuffer,
    uint8_t size
){
    char hexString[size * 2 + 1] = {0};  // 2 chars per byte + null terminator
    for (size_t i = 0; i < size; ++i) {
        snprintf(hexString + (i * 2), 3, "%02X", packetBuffer[i]);
    }

    oled.drawString(0, 20, hexString);
    oled.display();
}

/*
void
update_display(
    void
){
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

void
rgb_led(
    uint8_t red,
    uint8_t green,
    uint8_t blue
){
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
}*/

extern Service displayServiceDesc;

void
display_init(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("OLED Init...\r\n");
#endif // DEBUG_ON

    VextON();// oled power on;
    delay(100);

    oled.init();
    delay(100);

    oled.wakeup();

    oled.clear();
    oled.setFont(ArialMT_Plain_10);

    // oled.drawRect(0,0, 16, 16);
    oled.drawXbm(0, 0, NR_diagonal_width, NR_diagonal_height, NR_diagonal_bits);
    oled.display();
}

void
displayService(
    void
){
    displayServiceDesc.busy = false;

    static int old_modem_id = 257;

    if (old_modem_id != get_modem_id()){
        old_modem_id = get_modem_id();
    }

}