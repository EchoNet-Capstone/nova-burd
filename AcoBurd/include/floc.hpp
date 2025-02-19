#ifndef FLOC_H
#define FLOC_H

#include <Arduino.h>
#include <stdint.h>
#include "globals.hpp"
#include "modem_api.hpp"

#define COMMAND_TYPE 0x1
#define ACK_TYPE 0x2
#define RESPONSE_TYPE 0x3

struct header_common {
    uint8_t ttl : 4,
            type : 4;
    uint8_t dest_addr;
    uint8_t src_addr;
}__attribute__((packed));

struct command_header {
    uint8_t ttl : 4,
            type : 4;
    uint8_t dest_addr;
    uint8_t src_addr;
    uint8_t pid;
    uint8_t size;
}__attribute__((packed));

struct ack_header {
    uint8_t ttl : 4,
            type : 4;
    uint8_t dest_addr;
    uint8_t src_addr;
    uint8_t pid;
    uint8_t ack_pid; // pid of the packet being acknowledged
}__attribute__((packed));

struct response_header {
    uint8_t ttl : 4,
            type : 4;
    uint8_t dest_addr;
    uint8_t src_addr;
    uint8_t pid;
    uint8_t request_pid;
    uint8_t size;
}__attribute__((packed));



void parse_floc_command_packet(char *broadcastBuffer, uint8_t size);
void parse_floc_acknowledgement_packet(char *broadcastBuffer);
void parse_floc_response_packet(char *broadcastBuffer);
void floc_broadcast_received(char *broadcastBuffer, uint8_t size);
void floc_unicast_received(char *unicastBuffer, uint8_t size);
void floc_acknowledgement_send(HardwareSerial connection, uint8_t dest_addr, uint8_t ack_pid);
void floc_status_queue(HardwareSerial connection, uint8_t dest_addr);
void floc_status_send(String status);


#endif