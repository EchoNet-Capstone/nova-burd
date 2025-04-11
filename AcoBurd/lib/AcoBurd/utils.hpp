#ifndef UTILS_HPP
#define UITLS_HPP

#include <stdint.h>

int fieldToInt(char* field, uint8_t field_len);
void printFullPacket(uint8_t* pkt, uint8_t size);

#endif