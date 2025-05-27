#include <Arduino.h>
#include <HT_SSD1306Wire.h>

#include <stdlib.h>

#include <burd_EEPROM.hpp>

SSD1306Wire oled(0x3c, 500000, SDA, SCL, GEOMETRY_128_64, GPIO10); // addr , freq , SDA, SCL, resolution , rst

// State tracking
enum 
BurninState {
    WAITING_FOR_CLEAR_CONFIRMATION,  // Waiting for button press to clear
    WAITING_FOR_SERIAL_INPUT,        // Waiting for serial number input
    SERIAL_NUMBER_SET                // Process complete
};

static BurninState state = WAITING_FOR_CLEAR_CONFIRMATION;
static char buffer[10] = {0}; // Space for 8 hex chars + terminator
static int buffer_idx = 0;

void 
setup(
    void
){
    Serial.begin();

    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);
    delay(100);

    // Initialize USER_KEY pin as input with pull-up
    pinMode(USER_KEY, INPUT_PULLUP);

    oled.init();
    delay(100);

    oled.clear();
    oled.setFont(ArialMT_Plain_10);
    oled.drawString(0, 0, "Press USER_KEY to");
    oled.drawString(0, 13, "clear Serial Number");
    oled.display();
    
    Serial.println("Press USER_KEY to clear Serial Number");
    
    // Note: We don't clear the serial number here anymore
    // That will happen after button confirmation
}

void loop(void) {
    switch(state) {
        case WAITING_FOR_CLEAR_CONFIRMATION:
            // Wait for button press to clear serial number
            if (digitalRead(USER_KEY) == LOW) {
                // Debounce
                delay(50);
                if (digitalRead(USER_KEY) == LOW) {
                    // Wait for release
                    while (digitalRead(USER_KEY) == LOW) {
                        delay(10);
                    }
                    
                    // Clear the serial number now that user has confirmed
                    EEPROM_clearSerialNumber();
                    
                    // Update display
                    oled.clear();
                    oled.setFont(ArialMT_Plain_10);
                    oled.drawString(0, 0, "Serial Number cleared");
                    oled.drawString(0, 13, "Waiting for new");
                    oled.drawString(0, 26, "Serial Number...");
                    oled.display();
                    
                    Serial.println("Serial Number cleared. Waiting for new Serial Number...");
                    
                    // Move to next state
                    state = WAITING_FOR_SERIAL_INPUT;
                    buffer_idx = 0;  // Reset buffer
                }
            }
            break;
            
        case WAITING_FOR_SERIAL_INPUT:
            // Process incoming serial data
            while (Serial.available() > 0) {
                char nextChar = Serial.read();
                
                // Look for line ending
                if (nextChar == '\n' || nextChar == '\r') {
                    // Terminate string and process
                    buffer[buffer_idx] = '\0';
                    
                    // Convert ASCII to number (supports hex with 0x prefix)
                    uint32_t newSerialNumber = strtoul(buffer, NULL, 0); 
                    
                    // Set new serial number
                    EEPROM_setSerialNumber(newSerialNumber);
                    
                    // Update display
                    oled.clear();
                    oled.setFont(ArialMT_Plain_10);
                    oled.drawString(0, 0, "Serial Number:");
                    oled.drawString(0, 13, String(newSerialNumber, HEX));
                    oled.drawString(0, 26, "Successfully saved!");
                    oled.display();
                    
                    Serial.printf("New serial number set: 0x%08X\n", newSerialNumber);
                    
                    // Mark process as complete
                    state = SERIAL_NUMBER_SET;
                    break;
                } 
                else if (buffer_idx < sizeof(buffer) - 1) {
                    buffer[buffer_idx++] = nextChar;
                }
            }
            break;
            
        case SERIAL_NUMBER_SET:
            // Nothing to do, process is complete
            break;
    }
}