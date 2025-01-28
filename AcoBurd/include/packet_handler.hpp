#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include "Arduino.h"
#include "globals.hpp"



void packet_recieved(String packetBuffer);
void broadcast(HardwareSerial connection, char *data, int8_t bytes);








#endif