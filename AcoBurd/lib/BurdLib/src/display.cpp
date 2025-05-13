#include <HT_SSD1306Wire.h>

#include "safe_arduino.hpp"

#include <stdio.h>

#include <floc.hpp>
#include <nmv3_api.hpp>

#include "device_state.hpp"
#include "display.hpp"
#include "motor.hpp"
#include "services.hpp"

#define NR_diagonal_width 16
#define NR_diagonal_height 16
static const uint8_t PROGMEM NR_diagonal_bits[] = {
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

#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST

#define boat_width 20
#define boat_height 20
static const uint8_t PROGMEM boat_bits[] = {
   0x08,0x00,0x00,  /* row 0:  ................0000_1000 (top of mast) */
   0x08,0x00,0x00,  /* row 1:  ................0000_1000 (mast) */
   0x08,0x00,0x00,  /* row 2:  ................0000_1000 (mast) */
   0x18,0x00,0x00,  /* row 3:  ................0001_1000 (mast with sail start) */
   0x38,0x00,0x00,  /* row 4:  ................0011_1000 (sail growing) */
   0x78,0x00,0x00,  /* row 5:  ................0111_1000 (sail growing) */
   0xF8,0x00,0x00,  /* row 6:  ................1111_1000 (sail full width) */
   0xF8,0x01,0x00,  /* row 7:  .............1_1111_1000 (sail with hull start) */
   0xF8,0x03,0x00,  /* row 8:  ............11_1111_1000 (hull growing) */
   0xF8,0x07,0x00,  /* row 9:  ...........111_1111_1000 (hull growing) */
   0xF8,0x0F,0x00,  /* row 10: ..........1111_1111_1000 (hull growing) */
   0xF8,0x1F,0x00,  /* row 11: .........11111_1111_1000 (hull growing) */
   0xF8,0x3F,0x00,  /* row 12: ........111111_1111_1000 (hull growing) */
   0xF8,0x7F,0x00,  /* row 13: .......1111111_1111_1000 (hull growing) */
   0xF8,0xFF,0x00,  /* row 14: ......11111111_1111_1000 (full width hull) */
   0x00,0x00,0x00,  /* row 15: .......................... (water line) */
   0xFF,0xFF,0x00,  /* row 16: ......11111111_11111111 (wave) */
   0xFF,0x7F,0x00,  /* row 17: .......1111111_11111111 (wave) */
   0xFF,0x3F,0x00,  /* row 18: ........111111_11111111 (wave) */
   0x00,0x00,0x00,  /* row 19: .......................... (empty space) */
};

#else

#define buoy_width 24
#define buoy_height 20
static const uint8_t PROGMEM buoy_bits[] = {
   0x00,0x00,0x00,  /* row  0: ............................. (empty top) */
   0x80,0xFF,0x01,  /* row  1: 0000_0001.1111_1111.1000_0000 (top signal wave) */
   0x70,0x00,0x0E,  /* row  2: 0000_1110.0000_0000.0111_0000 (outer signal wave) */
   0x0C,0x7E,0x30,  /* row  3: 0011_0000.0111_1110.0000_1100 (edges of outer wave) */
   0xC4,0x81,0x23,  /* row  4: 0010_0011.1000_0001.1100_0100 (middle signal wave) */
   0x30,0x00,0x0C,  /* row  5: 0000_1100.0000_0000.0011_0000 (signal wave) */
   0x10,0x18,0x08,  /* row  6: 0000_1000.0001_1000.0001_0000 (signal wave) */
   0x00,0x3C,0x00,  /* row  7: ..........0011_1100.......... (antenna top) */
   0x00,0x7E,0x00,  /* row  8: ..........0111_1110.......... (antenna circle) */
   0x00,0x3C,0x00,  /* row  9: ..........0011_1100.......... (antenna bottom) */
   0x00,0x18,0x00,  /* row 10: ..........0001_1000.......... (antenna pole) */
   0x00,0x18,0x00,  /* row 11: ..........0001_1000.......... (buoy top) */
   0x00,0x7E,0x00,  /* row 12: ..........0111_1110.......... (buoy structure) */
   0x80,0xDB,0x01,  /* row 13: ..........1001_1001.......... (buoy structure) */
   0x62,0x18,0x86,  /* row 14: 0100_0011.1111_1111.1100_0001 (buoy base) */
   0x73,0x18,0x4E,  /* row 15: 1100_1111.1111_1111.1111_0010 (buoy base wider) */
   0xFA,0xFF,0x9F,  /* row 16: 0101_1111.1111_1111.1111_1001 (buoy base widest) */
   0x00,0x00,0x00,  /* row 17: ............................. (water) */
   0x78,0x2B,0x1A,  /* row 18: 0001_1110.1101_0100.0101_1000 (wave under buoy) */
   0x00,0x00,0x00,  /* row 19: ............................. (empty bottom) */
};

#endif // RECV_SERIAL_NEST

const uint8_t logoPos[] = { 4, 4 };

const uint8_t logoCircle[] = { 12, 12, 12 };

const uint8_t echoNetLogoPos[] = { 100, 40 };

const uint8_t logoRightBorder[] = { 25, 0, 25};

const uint8_t topLine[] = {0, 25, 128 };

const uint8_t vertSep[] = { 79, 25, 13 };

const uint8_t vertSep2[] = { 95, 38, 25 };

const uint8_t bottomLine[] = {0, 38, 128 };

#define TEXT_BUFF_SIZE 40

typedef struct TextArea_t {
    const uint8_t x;
    const uint8_t y;
    const uint8_t* font;
    const uint8_t clearOffsetY;
    const uint8_t width; // Maximum expected width or 0 for auto-calculate
    char currentText[TEXT_BUFF_SIZE];
    char newText[TEXT_BUFF_SIZE];
};

TextArea_t batteryText = {
    .x = 28,
    .y = 3,
    .font = ArialMT_Plain_16,
    .clearOffsetY = 0,
    .width = 0,
    .currentText = {0},
    .newText = {0}
};

TextArea_t modemIdText = {
    .x = 0,
    .y = 25,
    .font = ArialMT_Plain_10,
    .clearOffsetY = 1,
    .width = 0,
    .currentText = {0},
    .newText = {0}
};

TextArea_t motorStatusText = {
    .x = 84,
    .y = 25,
    .font = ArialMT_Plain_10,
    .clearOffsetY = 1,
    .width = 0,
    .currentText = {0},
    .newText = {0}
};

TextArea_t networkIdText = {
    .x = 0,
    .y = 39,
    .font = ArialMT_Plain_10,
    .clearOffsetY = 0,
    .width = 0,
    .currentText = {0},
    .newText = {0}
};

TextArea_t deviceIdText = {
    .x = 0,
    .y = 51,
    .font = ArialMT_Plain_10,
    .clearOffsetY = 1,
    .width = 0,
    .currentText = {0},
    .newText = {0}
};

SSD1306Wire oled(0x3c, 500000, SDA, SCL, GEOMETRY_128_64, GPIO10); // addr , freq , SDA, SCL, resolution , rst

void
updateTextArea(
    TextArea_t& area
){
    int clearWidth = area.width;

    if( clearWidth == 0 ){
        int oldWidth = oled.getStringWidth(area.currentText);
        int newWidth = oled.getStringWidth(area.newText);

        clearWidth = (oldWidth > newWidth) ? oldWidth : newWidth;
    }

    oled.setColor(BLACK);
    oled.fillRect(area.x, area.y + area.clearOffsetY, clearWidth, area.font[1] - area.clearOffsetY);

    oled.setColor(WHITE);
    oled.setFont(area.font);
    oled.drawString(area.x, area.y, area.newText);

    strcpy(area.currentText, area.newText);
}

void
draw_battery_pct(
    void
){
    memset(batteryText.newText, 0, TEXT_BUFF_SIZE);

    sprintf(batteryText.newText, "Battery: %d%%", get_battery_percent());

    updateTextArea(batteryText);
}

void
draw_modem_id(
    void
){
    memset(modemIdText.newText, 0, TEXT_BUFF_SIZE);

    if(get_modem_id_set()){
        sprintf(modemIdText.newText, "Modem ID: %d", get_modem_id());
    }else{
        sprintf(modemIdText.newText, "Modem ID: UKN");
    }

    updateTextArea(modemIdText);
}

void
draw_motor_status(
    void
){
    memset(motorStatusText.newText, 0, TEXT_BUFF_SIZE);

    switch(get_motor_status()){
        case STOPPED:
            sprintf(motorStatusText.newText, "Stopped");
            break;

        case RUNNING:
            sprintf(motorStatusText.newText, "Running");
            break;

        case WIGGLING:
            sprintf(motorStatusText.newText, "Wiggling");
            break;

        default:
            sprintf(motorStatusText.newText, "Unknown");
            break;
    }

    updateTextArea(motorStatusText);
}

void
draw_device_id(
    void
){
    memset(deviceIdText.newText, 0, TEXT_BUFF_SIZE);

    uint16_t deviceId = get_device_id();

    sprintf(deviceIdText.newText, "Device ID:   %02X_%02X", (uint8_t) ((deviceId >> 8) & 0xFF), (uint8_t) (deviceId & 0xFF));

    updateTextArea(deviceIdText);
}

void
draw_network_id(
    void
){
    memset(networkIdText.newText, 0, TEXT_BUFF_SIZE);

    uint16_t networkId = get_network_id();

    sprintf(networkIdText.newText, "Network ID: %02X_%02X", (uint8_t) ((networkId >> 8) & 0xFF), (uint8_t) (networkId & 0xFF));

    updateTextArea(networkIdText);
}

void
draw_main_screen(
    void
){
    // lines 
    oled.drawVerticalLine(logoRightBorder[0], logoRightBorder[1], logoRightBorder[2]);
    oled.drawVerticalLine(vertSep[0], vertSep[1], vertSep[2]);
    oled.drawVerticalLine(vertSep2[0], vertSep2[1], vertSep2[2]);
    oled.drawHorizontalLine(topLine[0], topLine[1], topLine[2]);
    oled.drawHorizontalLine(bottomLine[0], bottomLine[1], bottomLine[2]);

    // logo
    oled.drawXbm(logoPos[0], logoPos[1], NR_diagonal_width, NR_diagonal_height, NR_diagonal_bits);
    // oled.drawCircle(logoCircle[0], logoCircle[1], logoCircle[2]);

    // echoNet logo
#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
    oled.drawXbm(echoNetLogoPos[0], echoNetLogoPos[1], boat_width, boat_height, boat_bits);
#else
    oled.drawXbm(echoNetLogoPos[0], echoNetLogoPos[1], buoy_width, buoy_height, buoy_bits);
#endif // RECV_SERIAL_NEST

    draw_modem_id();
    draw_device_id();
    draw_network_id();

#ifndef RECV_SERIAL_NEST // !RECV_SERIAL_NEST
    draw_battery_pct();
    draw_motor_status();
#endif // !RECV_SERIAL_NEST

    oled.display();
}

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
oled_off(
    void
){
    oled.sleep();
}

void
oled_on(
    void
){
    oled.wakeup();
}

void
oled_sleep(
    void
){
    oled.stop();
}

void
oled_wakeup(
    void
){
    oled.connect();

    oled.init();
    delay(100);

    oled.clear();

    draw_main_screen();
}

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

    oled.clear();

    draw_main_screen();
}

void
displayService(
    void
){
    displayServiceDesc.busy = false;

    static bool changed = false;
    
    static int old_modem_id = 257;
    static int old_motor_status = -1;
    static int old_battery_pct = -1;

    if (old_modem_id != get_modem_id()){
        old_modem_id = get_modem_id();

        draw_modem_id();
        
        changed = true;
    }

#ifndef RECV_SERIAL_NEST // !RECV_SERIAL_NEST
    if (old_battery_pct != get_battery_percent()){
        old_battery_pct = get_battery_percent();

        draw_battery_pct();

        changed = true;
    }

    if (old_motor_status != get_motor_status()){
        old_motor_status = get_motor_status();

        draw_motor_status();

        changed = true;
    }
#endif // !RECV_SERIAL_NEST

    if(changed){
        oled.display();

        displayServiceDesc.busy = true;

        changed = false;
    }
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

/*void
display_modem_packet_data(
    String packetBuffer
){
        oled.drawString(0, 20, "  Packet : " + packetBuffer);
        oled.display();
}*/

// void
// display_modem_packet_data(
//     uint8_t *packetBuffer,
//     uint8_t size
// ){
//     char hexString[size * 2 + 1] = {0};  // 2 chars per byte + null terminator
//     for (size_t i = 0; i < size; ++i) {
//         snprintf(hexString + (i * 2), 3, "%02X", packetBuffer[i]);
//     }

//     oled.drawString(0, 20, hexString);
//     oled.display();
// }