/* 
 * The purpose of this is to reduce overhead and allow the device to 
 * filter out messages it has already seen
 * 
 * */
#include "bloomfilter.hpp"
#include <bits/stdc++.h>

#define BLOOM_FILTER_BITS 64
#define BLOOM_FILTER_BYTES (BLOOM_FILTER_BITS / 8)

static uint8_t bloom_filter[BLOOM_FILTER_BYTES] = {0};

uint8_t 
hash1(
    uint32_t key
) {
    return (key * 31) % 64;
}

uint8_t 
hash2(
    uint32_t key
) {
    return ((key >> 3) ^ (key * 17)) % 64;
}

bool bloom_check(uint32_t key) {
    return (bloom_filter[hash1(key) / 8] & (1 << (hash1(key) % 8))) &&
           (bloom_filter[hash2(key) / 8] & (1 << (hash2(key) % 8)));
}


uint32_t 
hash_packet_buffer(
    uint8_t* buf, 
    uint8_t size
) {
    uint32_t hash = 5381;
    for (int i = 0; i < size; i++) {
        hash = ((hash << 5) + hash) ^ buf[i]; // djb2 variant (fast and light)
    }
    return hash;
}

bool 
bloom_check_packet(
    uint8_t* packetBuffer,
     uint8_t size
) {
    uint32_t key = hash_packet_buffer(packetBuffer, size); // or make_key_from_buffer
    return bloom_check(key);
}

void bloom_add_packet(
    uint8_t* packetBuffer,
    uint8_t size
) {
    uint32_t key = hash_packet_buffer(packetBuffer, size); // or make_key_from_buffer
    bloom_add(key);
}

void 
bloom_add(
    uint32_t key
) {
    bloom_filter[hash1(key) / 8] |= (1 << (hash1(key) % 8));
    bloom_filter[hash2(key) / 8] |= (1 << (hash2(key) % 8));
}