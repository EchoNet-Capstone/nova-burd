#include <Arduino.h>
#include "heltec_serial_api.hpp"
#include "display.hpp"
#include "motor.hpp"
#include "watchdog.hpp"

// Defined if the serial port (not serial1) is used to receive data from the NeST
#define RECV_SERIAL_NEST

// Testing define
// #define MASTER_NODE

// Packet buffer for data received from the ship terminal (NeST) serial line
String packetBuffer_nest = "";

// Packet buffer for data received from the acoustic modem serial line
String packetBuffer_modem = "";


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

  // motor_init();

  //Enable the WDT.
  // innerWdtEnable(true);

  // VextOFF();

  // init_sleep();

  // go_to_sleep();

  oled_initialize();

  interrupts();

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
    struct command_header *temp = (struct command_header *)malloc(sizeof(struct command_header));
    temp->common.ttl = 5;
    temp->common.type = COMMAND_TYPE;
    temp->common.dest_addr = 2;
    temp->common.src_addr = 1;
    temp->pid = 4;
    temp->size = 3;

    char *struct_ptr = (char *)temp;
    // structure acoustic broadcast command
    MODEM_SERIAL_CONNECTION.print("$B05");
    // Send command data as packet in broadcast
    MODEM_SERIAL_CONNECTION.print(struct_ptr);


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
    if (NEST_SERIAL_CONNECTION.available() > 0) {
        char nest_char = NEST_SERIAL_CONNECTION.read();  // Read one character from modem
        packetBuffer_nest += nest_char;

        // Check for <CR><LF> sequence
        int end_sequence = packetBuffer_nest.indexOf("\r\n");
        if (end_sequence > -1) {
            String current_packet_nest = packetBuffer_nest.substring(0, end_sequence);
            packetBuffer_nest = packetBuffer_nest.substring(end_sequence + 2);

            packet_received_nest(current_packet_nest);
        }
    }
#endif

    if (MODEM_SERIAL_CONNECTION.available() > 0) {
        char modem_char = MODEM_SERIAL_CONNECTION.read();  // Read one character from modem
        packetBuffer_modem += modem_char;

        // Check for <CR><LF> sequence
        int end_sequence = packetBuffer_modem.indexOf("\r\n");
        if (end_sequence > -1) {
            String current_packet_modem = packetBuffer_modem.substring(0, end_sequence);
            packetBuffer_modem = packetBuffer_modem.substring(end_sequence + 2);

            packet_received_modem(current_packet_modem);
        }
    }
}
