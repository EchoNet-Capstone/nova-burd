#ifndef GLOBALS_H
#define GLOBALS_H

// PREVIOUS INCLUDES DO NOT DELETE
// #include <Arduino.h>
// #include "watchdog.h"
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

#define UNIT_ID 1                             // Set individual hardware number
#define DEBUG false                           // Debug mode
#define LEAK_DETECT true                      // Leak Detection
#define LORA_ENABLE false                     // LoRa Radio
#define GPS_ENABLE false                      // GPS
#define LED_ENABLE false                      // LED Strobe
#define HALL_EFFECT true                      // Hall effect inputs or reed switches

#endif