#ifndef RADIO_H
#define RADIO_H

#include "main.h"

void OnTxDone(void);
void OnTxTimeout(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

#endif