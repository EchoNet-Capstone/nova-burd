#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include "Arduino.h"
#include "globals.hpp"


void print_packet(String packetBuffer, String packet_type);
void query_status(HardwareSerial connection);
void set_address(HardwareSerial connection, int8_t addr);
void broadcast(HardwareSerial connection, char *data, int8_t bytes);
void ping(HardwareSerial connection, int8_t addr);
void parse_status_query_packet(String packetBuffer);
void parse_broadcast_packet(String packetBuffer);
void parse_unicast_packet(String packetBuffer);
void packet_recieved(String packetBuffer);






#endif