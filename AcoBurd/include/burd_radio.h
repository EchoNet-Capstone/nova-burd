#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>


// LoRa pin definitions
#define LORA_IO1
#define LORA_IO2
#define LORA_RESET
#define LORA_SEL
#define LORA_SCK CLK1
#define LORA_MOSI MOSI1
#define LORA_MISO MISO1
#define LORA_IO0

// LoRa config
#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             14        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       12        // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000
#define LORA_BUFFER_SIZE                            64          // Define the payload size here

void radio_init(void);
void OnTxDone(void);
void OnTxTimeout(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

#endif