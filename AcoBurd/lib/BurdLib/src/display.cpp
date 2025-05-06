#include <CubeCell_NeoPixel.h>
#include <HT_SSD1306Wire.h>

#include "safe_arduino.hpp"

#include <stdio.h>

#include <nmv3_api.hpp>

#include "display.hpp"
#include "services.hpp"

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
oled_initialize(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("OLED Init...\r\n");
#endif // DEBUG_ON

    VextON();// oled power on;
    delay(10);

    oled.init();
    oled.clear();
    oled.setFont(ArialMT_Plain_10);
}

void
display_modem_id(
    void
){
    oled.clear();

    if(get_modem_id_set()){
        oled.drawString(0, 0, "  Modem ID : " + String(get_modem_id(), DEC));
    }else{
        oled.drawString(0, 0, "  Modem ID : UNKNOWN");
    }

    oled.display();
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
displayService(
    void
){
    displayServiceDesc.busy = false;

    static int old_modem_id = -1;

    if ( old_modem_id != get_modem_id()){
        old_modem_id = get_modem_id();

        display_modem_id();
    }

}