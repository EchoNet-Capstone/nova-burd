#ifndef FLOC_H
#define FLOC_H

#include <stdint.h>
#include "globals.hpp"

struct header_common {
    uint8_t ttl : 4,
            type : 4;
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
}__attribute__((packed));

struct response_header {
    uint8_t ttl : 4,
            type : 4;
    uint8_t dest_addr;
    uint8_t src_addr;
    uint8_t pid;
    uint8_t size;
}__attribute__((packed));



void floc_broadcast_received(char *broadcastBuffer);
void floc_unicast_received();




#endif