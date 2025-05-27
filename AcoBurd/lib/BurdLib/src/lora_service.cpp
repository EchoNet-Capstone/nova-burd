#include <LoRaWan_APP.h>

#include "safe_arduino.hpp"

#include "my_clock.hpp"
#include "services.hpp"

// LoRa parameters - must be the same on all communicating devices
#define RF_FREQUENCY 915000000  // Hz (US915 band)
#define TX_OUTPUT_POWER 14      // dBm
#define LORA_BANDWIDTH 0        // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz]
#define LORA_SPREADING_FACTOR 7 // [SF7..SF12]
#define LORA_CODINGRATE 1       // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH 8  // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0   // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#define RX_TIMEOUT_VALUE   1000       // ms

#define TX_WAIT_VALUE 5000 //ms

RadioEvents_t radioEvents;

// Test packet content
uint8_t testPacket[] = "Hello LoRa world!";
bool txDone = false;

// Callback implementations for RadioEvents
void 
OnTxDone(
    void
) {
    Serial.println("TX done");
    txDone = true;
    
    Radio.Standby();
}

void 
OnTxTimeout(
    void
) {
    Serial.println("TX timeout");
    txDone = true;
    
    Radio.Standby();
}

void 
OnRxDone(
    uint8_t *payload,
    uint16_t size,
    int16_t rssi,
    int8_t snr
) {
    // Create a null-terminated string from the payload
    char message[256];
    memcpy(message, payload, size);
    message[size] = '\0';
    
    Serial.print("Received packet: '");
    Serial.print(message);
    Serial.print("', RSSI: ");
    Serial.print(rssi);
    Serial.print(" dBm, SNR: ");
    Serial.print(snr);
    Serial.println(" dB");
    
    Radio.Standby();
}

void 
OnRxTimeout(
    void
) {
    Serial.println("RX timeout");
    
    Radio.Standby();
}

void 
OnRxError(
    void
) {
    Serial.println("RX error");

    Radio.Standby();
}

// Function to send a test packet
void 
sendTestPacket(
    void
) {
    Serial.println("Sending LoRa test packet...");
    
    // Send the test packet via LoRa
    Radio.Send(testPacket, sizeof(testPacket));
}

extern Service loraServiceDesc;

void
loraService(
    void
){
    loraServiceDesc.busy = false;

    static int  numTx = 0;
    static long txTimer = InternalClock();

    // If the last transmission is complete, send another test packet
    if (txDone && numTx < 5 && (int32_t)(InternalClock() - txTimer) >= 0) {
        txDone = false;
        Serial.println("Sending another test packet...");
        sendTestPacket();

        numTx ++;
        txTimer = InternalClock() + TX_WAIT_VALUE;

        loraServiceDesc.busy = true;
    }
}

void
lora_init(
    void
){
    // Setup the callbacks
    radioEvents.TxDone = OnTxDone;
    radioEvents.TxTimeout = OnTxTimeout;
    radioEvents.RxDone = OnRxDone;
    radioEvents.RxTimeout = OnRxTimeout;
    radioEvents.RxError = OnRxError;

    Radio.Init(&radioEvents);

    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                        LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                        true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
  
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                        LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                        LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                        0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

    // Send a test packet immediately
    sendTestPacket();
}