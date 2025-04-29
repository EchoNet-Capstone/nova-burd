#pragma once

#include <stdint.h>

int
fieldToInt(
    char* field,
    uint8_t field_len
);

void
printBufferContents(
    uint8_t* buf,
    uint8_t buf_size
);
