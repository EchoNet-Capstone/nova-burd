#include <LoRa_APP.h>

#include "burd_radio.h"
#include "device_state.h"
#include "my_clock.h"
#include "timers.h"
#include "display.h"

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

void radio_service(){
  // Send a LoRa packet
  if(LORA_ENABLE && get_waiting_to_be_retrieved() && (get_lora_timer() < InternalClock())){
    if (DEBUG){
      Serial.printf("Sending LoRa packet.\r\n");
    }

    // Need to power up Vext to supply power to LoRa radio
    oled_wake();

    // Use this for testing only
    set_lora_timer(InternalClock() + LORA_INTERVAL);
  }
  else {
    // Put LoRa to sleep if we're not sending a packet
    Radio.Sleep();
    // Increment this even if not tranmitting.  That way the LoRa radio won't transmit immediately upon activation (let float to surface) THIS BREAKS THINGS
    //lora_timer = InternalClock() + lora_interval;
  }
}

void OnTxDone(){
  // Flash blue LED to indicate packet has been sent
  rgb_led(0, 0, 16);
  Radio.Sleep( );
  if (DEBUG){
    Serial.print("LoRa TX sent......\r\n");
  }
  //state=RX;
}

void OnTxTimeout(){
  // Flash red LED to indicate failed packet
  rgb_led(16, 0, 0);
  Radio.Sleep( );
  if (DEBUG){
    Serial.print("LoRa TX Timeout......\r\n");
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