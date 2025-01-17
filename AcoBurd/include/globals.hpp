#ifndef GLOBALS_H
#define GLOBALS_H

// PREVIOUS INCLUDES DO NOT DELETE
// #include <Arduino.h>
// #include "innerWdt.h"
// #include <Wire.h>
// #include "HT_SSD1306Wire.h"
// #include "CubeCell_NeoPixel.h"
// #include "LoRaWan_APP.h"
// #include "LoRa_APP.h"
// #include "GPS_Air530.h"
// #include "GPS_Air530Z.h"

// Do not use pins ADC1, GPIO1, GPIO2, GPIO8, GPIO10, GPIO12, GPIO13, GPIO14, GPIO15, GPIO16, GPIO19, GPIO20, 
// Note:  AB-02S uses ASR6502 chip, not ASR6501.  ASR6502 has three working ADC channels. 
//
// GPIO12 GPS Green LED
// GPIO13 SK6812 LED
// GPIO14 GPS Power
// GPIO15 Vext
//
//  ANALOG = 0,              brief High Impedance Analog
//  INPUT,                   brief High Impedance Digital
//  OUTPUT_PULLUP ,          brief Resistive Pull Up
//  OUTPUT_PULLDOWN ,        brief Resistive Pull Down
//  OD_LO ,                  brief Open Drain, Drives Low
//  OD_HI ,                  brief Open Drain, Drives High
//  OUTPUT ,                 brief Strong Drive
//  OUTPUT_PULLUP_PULLDOWN , brief Resistive Pull Up/Down

#define unit_id 1                             // Set individual hardware number
#define debug false                           // Debug mode
#define leak_detect true                      // Leak Detection
#define lora_enable false                     // LoRa Radio
#define gps_enable false                      // GPS
#define led_enable false                      // LED Strobe
#define hall_effect true                      // Hall effect inputs or reed switches

// Various Calibrations
#define battery_full 4050                     // Voltage for full battery - was 4200 (this resulted in full battery being 95%, 4140 is actual measured full voltage
#define battery_empty 3350                    // Voltage for empty battery - 3300 for nearly empty at 0C (https://www.richtek.com/Design%20Support/Technical%20Document/AN024)
#define low_battery 5                         // Low battery condition in percent
#define battery_interval 180                  // Time between battery samples
#define gps_interval 3600                     // Time between GPS updates
#define lora_interval 60                      // Time between LoRa transmits
#define wiggle_interval 259200                // 86400 seconds in a day, 3 * 86400 = 259200
#define wiggle_deadband 259200                // If release will occur in next x seconds, then don't wiggle

// Timer Add Configuration
#define release_timer_add_1 7200              // Time to add for each magnet tap
#define release_timer_add_2 86400             // Time to add for each magnet tap
#define release_timer_first_press_1 120        // First tap starts clock at this number
#define release_timer_first_press_2 172830      // First tap starts clock at this number (add an extra 30 seconds for readability)
#define display_timeout 20                    // Must be bigger than reset timer (15s)
#define encoder_timeout 10

// Motor and Gearbox Configuration
#define gearbox_ratio 499
#define pulses_per_motor_rotation 11
#define motor_deadband 50
#define open_position 0

//#define closed_position (0.490 * gearbox_ratio * pulses_per_motor_rotation)                    // Silver AliExpress Motors - 2695 counts per rev
#define closed_position (0.550 * gearbox_ratio * pulses_per_motor_rotation)                    // Red Mark Pololu Motors

// LoRa pin definitions
#define LORA_IO1
#define LORA_IO2
#define LORA_RESET
#define LORA_SEL
#define LORA_SCK CLK1
#define LORA_MOSI MOSI1
#define LORA_MISO MISO1
#define LORA_IO0

// LoRa config
#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             14        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       12        // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 64 // Define the payload size here

// Interface GPIOs
#define INT_GPIO USER_KEY
#define reed_switch_input1 GPIO1
#define reed_switch_input_int INT_GPIO        // Don't use this - causes tons of random interrupts
#define reed_switch_input2 GPIO2
#define reed_switch_calibrate 25
#define reed_switch_super_long_press 60
#define reed_switch_long_press 15
#define reed_switch_short_press 1

// Motor and Encoder GPIO Setup
#define motor_driver_power GPIO7              // To motor driver Vcc AND motor encoder blue wire (encoder power)
#define motor_driver_a GPIO5                  // Motor driver ENABLE (Must be HIGH)
#define motor_driver_b GPIO6                  // Motor driver PHASE (Direction)
#define motor_quad_a GPIO3                    // Motor encoder Yellow
#define motor_quad_b GPIO4                    // Motor encoder White


#define gps_power GPIO14                      // Power control for Air530 GPS Module - active low


// Variables








 //long gps_timer;
 //
 //long led_timer;
 //long last_gpio_interrupt;



 long InternalClock(void);




#endif