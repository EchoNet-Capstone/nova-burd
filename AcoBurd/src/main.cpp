#include <Arduino.h>
#include "packet_handler.hpp"


String packetBuffer = "";

void setup(){
  
  // Debug messages to USB connection
  Serial.begin(115200);

  // Serial connection to modem
  Serial1.begin(9600, SERIAL_8N1);


  if (Serial1.availableForWrite()) {
    Serial.println("Serial1 Available, transmitting");
    // Serial1.write("$B04HEYO");
    char *hw = "hello world";
    // broadcast(Serial1, hw, 10);
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
