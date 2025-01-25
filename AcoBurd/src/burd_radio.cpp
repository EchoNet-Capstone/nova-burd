#include <LoRa_APP.h>

#include "burd_radio.h"
#include "display.h"
#include "globals.h"

static RadioEvents_t radio_events;

void radio_init(void){
  radio_events.TxDone = OnTxDone;
  radio_events.TxTimeout = OnTxTimeout;
  radio_events.RxDone = OnRxDone;

  Radio.Init( &radio_events );
  Radio.SetChannel( RF_FREQUENCY );
  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                     LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                     LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                     true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

  Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                     LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                     LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                     0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

  Radio.Sleep();
}

void OnTxDone(){
  rgb_led(0, 0, 16);                                                        // Flash blue LED to indicate packet has been sent
  Radio.Sleep( );
  if (DEBUG){
    Serial.print("LoRa TX sent......");
  }
  //state=RX;
}

void OnTxTimeout(){
  rgb_led(16, 0, 0);                                                        // Flash red LED to indicate failed packet
  Radio.Sleep( );
  if (DEBUG){
    Serial.print("LoRa TX Timeout......");
  }
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){
  char lora_rx_packet[LORA_BUFFER_SIZE];

  memcpy(lora_rx_packet, payload, size );
  lora_rx_packet[size]='\0';
  Radio.Sleep( );

  if(DEBUG){
    Serial.printf("\r\nreceived packet \"%s\" with rssi %d , length %d\r\n", lora_rx_packet, rssi, size);
  }
  if(DEBUG){
    Serial.println("wait to send next packet");
  }
}
