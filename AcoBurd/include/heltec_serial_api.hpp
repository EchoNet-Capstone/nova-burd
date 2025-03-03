#ifndef HELTEC_SERIAL_API_H
#define HELTEC_SERIAL_API_H

#include "Arduino.h"
#include "globals.hpp"
#include "floc.hpp"
#include "display.hpp"


void print_packet(String packetBuffer, String packet_type);
void query_status(HardwareSerial connection);
void set_address(HardwareSerial connection, uint8_t addr);
uint8_t get_modem_address();
void broadcast(HardwareSerial connection, char *data, uint8_t bytes);
void ping(HardwareSerial connection, uint8_t addr);
void parse_status_query_packet(uint8_t* packetBuffer, uint8_t size);
void parse_broadcast_packet(uint8_t* packetBuffer, uint8_t size);
void parse_unicast_packet(uint8_t* packetBuffer, uint8_t size);
void packet_received_modem(uint8_t* packetBuffer, uint8_t size);
void packet_received_nest(uint8_t* packetBuffer, uint8_t size);


#endif