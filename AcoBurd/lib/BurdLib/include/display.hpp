#pragma once

#include <stdint.h>

void
VextON(
    void
);

void
VextOFF(
    void
);

void
oled_off(
    void
);

void
oled_on(
    void
);

void
oled_sleep(
    void
);

void
oled_wakeup(
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