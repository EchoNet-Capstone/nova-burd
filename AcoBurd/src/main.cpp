#include <Arduino.h>
#include "modem_api.hpp"
#include "display.hpp"


String packetBuffer = "";

void setup(){
  oled_initialize();
  
  // Debug messages to USB connection
  Serial.begin(115200);

  // Serial connection to modem
  Serial1.begin(9600, SERIAL_8N1);


  if (Serial1.availableForWrite()) {
    set_address(Serial1, 1);

    delay(300);
    query_status(Serial1);

    delay(300);
    ping(Serial1, 2);

    delay(300);
    struct command_header *temp = (struct command_header *)malloc(sizeof(struct command_header));
    temp->ttl = 5;
    temp->type = COMMAND_TYPE;
    temp->dest_addr = 2;
    temp->src_addr = 1;
    temp->pid = 4;
    temp->size = 3;
    Serial1.print("$B05");

    char *struct_ptr = (char *)temp;

    Serial1.print(struct_ptr);


  }

}

void loop(){

    if (Serial1.available() > 0) {
        char c = Serial1.read();  // Read one character from modem

        // Check for <CR><LF> sequence
        if (c == '\n' && packetBuffer.endsWith("\r")) {
            // Remove the <CR> from the buffer
            packetBuffer.remove(packetBuffer.length() - 1);
            
            // Full packet received
            packet_recieved(packetBuffer);
            packetBuffer = "";  // Clear the buffer
        } else {
            // Append character to the buffer
            packetBuffer += c;
        }
    }
}
