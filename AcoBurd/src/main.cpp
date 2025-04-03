#include <Arduino.h>
#include <nmv3_api.hpp>
#include <floc.hpp>
#include <display.hpp>
#include <motor.hpp>
#include <watchdog.hpp>

// Testing define
#define MASTER_NODE

#ifdef MASTER_NODE
  // Defined if the serial port (not serial1) is used to receive data from the NeST
  #define RECV_SERIAL_NEST
#endif

// Packet buffer for data received from the ship terminal (NeST) serial line
static uint8_t packetBuffer_nest[SERIAL_FLOC_MAX_SIZE] = {0};
static uint8_t packetBuffer_nest_idx = 0;

// Packet buffer for data received from the acoustic modem serial line
static uint8_t packetBuffer_modem[FLOC_MAX_SIZE] = {0};
static uint8_t packetBuffer_modem_idx = 0;


void setup(){

  // Debug messages to USB connection
  NEST_SERIAL_CONNECTION.begin(115200, SERIAL_8N1);

  // Serial connection to modem
  MODEM_SERIAL_CONNECTION.begin(9600, SERIAL_8N1);


  // TimerReset(0);
  // Hopefully reset onboard timers
  // boardInitMcu();

  delay(100);

  if(debug){
    Serial.printf("Booting up...\r\n");
  }

  noInterrupts();

  motor_init();

  //Enable the WDT.
  // innerWdtEnable(true);

  // VextOFF();

  // init_sleep();

  // go_to_sleep();

  interrupts();

  oled_initialize();

#ifdef MASTER_NODE

  if (MODEM_SERIAL_CONNECTION.availableForWrite()) {
    // Master node address will be 1
    set_address(MODEM_SERIAL_CONNECTION, 1);

    delay(500);
    query_status(MODEM_SERIAL_CONNECTION);

    delay(500);
    // If there is a slave node, ping address 2
    ping(MODEM_SERIAL_CONNECTION, 2);

    delay(500);
    // Allocate memory for the command packet
    /*FlocPacket_t *temp = (FlocPacket_t *)malloc(sizeof(FlocHeader_t) + sizeof(CommandHeader_t) + 3); // 3 bytes of command data

    // Populate the common header
    temp->header.ttl = 2;
    temp->header.type = FLOC_COMMAND_TYPE;  // Corrected type from COMMAND_TYPE
    temp->header.nid = htons(4);  // Example NID
    temp->header.pid = 8;
    temp->header.res = 0;
    temp->header.dest_addr = htons(16);
    temp->header.src_addr = htons(32);

    // Populate the command-specific header
    temp->payload.command.header.command_type = COMMAND_TYPE_1;  // Example command type
    temp->payload.command.header.size = 3;

    // Populate command data (example data)
    temp->payload.command.data[0] = 'A';
    temp->payload.command.data[1] = 'S';
    temp->payload.command.data[2] = 'D';

    const uint8_t *struct_ptr = (uint8_t *)temp;
    if (debug) {
        Serial.printf("PACKET HERE (%02u bytes): [", sizeof(FlocHeader_t) + sizeof(CommandHeader_t) + 3);
        for(int i = 0; i < sizeof(FlocHeader_t) + sizeof(CommandHeader_t) + 3; i++){
            Serial.printf("%02x, ", struct_ptr[i]);
        }
        Serial.printf("]\r\n");
    }

    // Structure acoustic broadcast command
    MODEM_SERIAL_CONNECTION.printf("$B%02d", sizeof(FlocHeader_t) + sizeof(CommandHeader_t) + 3);

    // Send command data as packet in broadcast
    MODEM_SERIAL_CONNECTION.write(struct_ptr, (size_t)(sizeof(FlocHeader_t) + sizeof(CommandHeader_t) + 3));

    // Free allocated memory
    free(temp);*/
  }
#else
  if (MODEM_SERIAL_CONNECTION.availableForWrite()) {
    set_address(MODEM_SERIAL_CONNECTION, 2);

    delay(300);
    query_status(MODEM_SERIAL_CONNECTION);

    delay(300);
    // If there is a master node, ping address 1
    ping(MODEM_SERIAL_CONNECTION, 1);
  }
#endif
}

void loop(){

#ifdef RECV_SERIAL_NEST
    while (NEST_SERIAL_CONNECTION.available() > 0) {
        char nest_char = NEST_SERIAL_CONNECTION.read();

        // Check for <CR><LF> sequence
        if (nest_char == '\n' && packetBuffer_nest_idx > 0 && packetBuffer_nest[packetBuffer_nest_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_nest[packetBuffer_nest_idx - 1] = 0;

            packet_received_nest(packetBuffer_nest, packetBuffer_nest_idx - 1);
            
            memset(packetBuffer_nest, 0 , sizeof(packetBuffer_nest)); // Clear the buffer
            packetBuffer_nest_idx = 0;
        } else {
            if (packetBuffer_nest_idx >= sizeof(packetBuffer_nest)) {
                // Some error has occurred, clear the packet
                memset(packetBuffer_nest, 0 , sizeof(packetBuffer_nest));
                packetBuffer_nest_idx = 0;
            }
            // Append character to the buffer
            packetBuffer_nest[packetBuffer_nest_idx] = nest_char;
            packetBuffer_nest_idx++;
        }
    }
#endif
    while (MODEM_SERIAL_CONNECTION.available() > 0) {
        char modem_char = MODEM_SERIAL_CONNECTION.read();

        // Check for <CR><LF> sequence
        if (modem_char == '\n' && packetBuffer_modem_idx > 0 && packetBuffer_modem[packetBuffer_modem_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_modem[packetBuffer_modem_idx - 1] = 0;

            packet_received_modem(packetBuffer_modem, packetBuffer_modem_idx - 1);
            
            memset(packetBuffer_modem, 0 , sizeof(packetBuffer_modem)); // Clear the buffer
            packetBuffer_modem_idx = 0;
        } else {
            if (packetBuffer_modem_idx >= sizeof(packetBuffer_modem)) {
                // Some error has occurred, clear the packet
                memset(packetBuffer_modem, 0 , sizeof(packetBuffer_modem));
                packetBuffer_modem_idx = 0;
            }
            // Append character to the buffer
            packetBuffer_modem[packetBuffer_modem_idx] = modem_char;
            packetBuffer_modem_idx++;
        }
    }
}
