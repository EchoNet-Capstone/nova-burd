#pragma once

#include <stdint.h>

void
oled_sleep(
    void
);

void
oled_wake(
    void
);

void
display_init(
    void
);

void
display_modem_packet_data(
    uint8_t *packetBuffer,
    uint8_t size
);

void
displayService(
    void
);