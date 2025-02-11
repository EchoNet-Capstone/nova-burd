#ifndef MODEM_API_H
#define MODEM_API_H

#include "Arduino.h"
#include "globals.hpp"
#include "floc.hpp"


void print_packet(String packetBuffer, String packet_type);
void query_status(HardwareSerial connection);
void set_address(HardwareSerial connection, int8_t addr);
uint8_t get_modem_address();
void broadcast(HardwareSerial connection, char *data, int8_t bytes);
void ping(HardwareSerial connection, int8_t addr);
void parse_status_query_packet(String packetBuffer);
void parse_broadcast_packet(String packetBuffer);
void parse_unicast_packet(String packetBuffer);
void packet_recieved(String packetBuffer);


#endif