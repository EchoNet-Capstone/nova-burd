#include <stdint.h>

#include <Arduino.h>

#include <device_actions.hpp>
#include <display.hpp>
#include <globals.hpp>
#include <motor.hpp>
#include <watchdog.hpp>
#include <buffer.hpp>
#include <activityperiod.hpp>

#include <floc.hpp>

#include <nmv3_api.hpp>

// Testing define

#ifdef MASTER_NODE // MASTER_NODE
    // Defined if the serial port (not serial1) is used to receive data from the NeST
    #define RECV_SERIAL_NEST
#endif // MASTER_NODE

// Packet buffer for data received from the ship terminal (NeST) serial line
static uint8_t packetBuffer_nest[SERIAL_FLOC_MAX_SIZE] = {0};
static uint8_t packetBuffer_nest_idx = 0;

// Packet buffer for data received from the acoustic modem serial line
static uint8_t packetBuffer_modem[FLOC_MAX_SIZE] = {0};
static uint8_t packetBuffer_modem_idx = 0;

FLOCBufferManager flocBuffer;

void 
setup(
    void
){

    // Debug messages to USB connection
    NEST_SERIAL_CONNECTION.begin(115200, SERIAL_8N1);

    // Serial connection to modem
    MODEM_SERIAL_CONNECTION.begin(9600, SERIAL_8N1);

    delay(100);

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Booting up...\r\n");
#endif // DEBUG_ON

    noInterrupts();

    motor_init();

    interrupts();

    oled_initialize();

    activitity_init();
}

void 
loop(
    void
){

    activity_update();

    if (is_activity_period_open() == SENDING) {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Activity period is open for sending...\r\n");
    #endif // DEBUG_ON

        if (flocBuffer.checkqueueStatus() == 0) {
        #ifdef DEBUG_ON // DEBUG_ON
            Serial.printf("No packets in the queue...\r\n");
        #endif // DEBUG_ON    

        } else {
        #ifdef DEBUG_ON // DEBUG_ON
            Serial.printf("Packets in the queue...\r\n");
        #endif // DEBUG_ON
        
            flocBuffer.queuehandler();
        }
    }

    // we are going to have a command activitiy variable 

#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
    while (NEST_SERIAL_CONNECTION.available() > 0) {
        char nest_char = NEST_SERIAL_CONNECTION.read();

        // Check for <CR><LF> sequence
        if (nest_char == '\n' && packetBuffer_nest_idx > 0 && packetBuffer_nest[packetBuffer_nest_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_nest[packetBuffer_nest_idx - 1] = 0;

            DeviceAction_t da;
            init_da(&da);

            packet_received_nest(packetBuffer_nest, packetBuffer_nest_idx - 1, &da);

            act_upon(&da);
            
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
#endif // RECV_SERIAL_NEST
    while (MODEM_SERIAL_CONNECTION.available() > 0) {
        char modem_char = MODEM_SERIAL_CONNECTION.read();

        // Check for <CR><LF> sequence
        if (modem_char == '\n' && packetBuffer_modem_idx > 0 && packetBuffer_modem[packetBuffer_modem_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_modem[packetBuffer_modem_idx - 1] = 0;

            DeviceAction_t da;
            init_da(&da);

            packet_received_modem(packetBuffer_modem, packetBuffer_modem_idx - 1, &da);

            act_upon(&da);
            
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
