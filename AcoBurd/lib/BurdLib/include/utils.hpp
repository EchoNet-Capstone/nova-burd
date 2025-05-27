#pragma once

#include <stdint.h>

uint8_t
modemIdFromDidNid(
    uint16_t deviceID,
    uint16_t networkID
);

void
printBufferContents(
    uint8_t* buf,
    uint8_t buf_size
);

static inline
uint16_t
htons(
    uint16_t val
){
    return __builtin_bswap16(val);
}

static inline
uint16_t
ntohs(
    uint16_t val
){
    return __builtin_bswap16(val);
}