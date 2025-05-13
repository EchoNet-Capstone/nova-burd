/* 
 * The purpose of this is to reduce overhead and allow the device to 
 * filter out messages it has already seen
 * 
 * */
#include "bloomfilter.hpp"
#include <bits/stdc++.h>

#define BLOOM_FILTER_SIZE 8 // 64 bits
static uint8_t bloom_filter[BLOOM_FILTER_SIZE];

uint8_t hash1(uint32_t key);
uint8_t hash2(uint32_t key);

uint8_t 
hash1(uint32_t key)
 {
    return (key * 31) % 64;
}

uint8_t 
hash2(uint32_t key)
 {
    return ((key >> 3) ^ (key * 17)) % 64;
}

bool 
bloom_check(
    uint32_t key
)  {
    bloom_filter[hash1(key) / 8] |= (1 << (hash1(key) % 8));
    bloom_filter[hash2(key) / 8] |= (1 << (hash2(key) % 8));
}

bool 
bloom_check(
    uint32_t key
) {
    return (bloom_filter[hash1(key) / 8] & (1 << (hash1(key) % 8))) &&
           (bloom_filter[hash2(key) / 8] & (1 << (hash2(key) % 8)));
}
