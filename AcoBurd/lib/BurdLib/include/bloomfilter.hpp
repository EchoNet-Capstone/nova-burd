#pragma once

#include <stdint.h>

uint32_t 
hash_packet_buffer(
    uint8_t* buf, 
    uint8_t size
);



bool 
bloom_check_packet(
    uint8_t* packetBuffer,
     uint8_t size
);


void bloom_add_packet(
    uint8_t* packetBuffer,
    uint8_t size
);

void 
bloom_add(
    uint32_t key
);