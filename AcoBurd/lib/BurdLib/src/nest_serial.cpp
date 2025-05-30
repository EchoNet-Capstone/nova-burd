#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
#include "safe_arduino.hpp"

#include <stdint.h>

#include <floc.hpp>
#include <nmv3_api.hpp>

#include "device_actions.hpp"
#include "globals.hpp"
#include "nest_serial.hpp"
#include "services.hpp"
#include "utils.hpp"

// Packet buffer for data received from the ship terminal (NeST) serial line
static uint8_t packetBuffer_nest[SERIAL_FLOC_MAX_SIZE] = {0};
static uint8_t packetBuffer_nest_idx = 0;

HardwareSerial& nest_connection = NEST_SERIAL_CONNECTION;

void
packet_received_nest(
    uint8_t* packetBuffer,
    uint8_t size
){

    if (size < 3) {
        // Need a prefix character, a casting type, and at least one byte of data e.g. $BX for a broadcast with data 'X'
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.println("NeST packet too small. Minimum size : 3.\r\n");
        printBufferContents(packetBuffer, size);
    #endif // DEBUG_ON

        return;
    }

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("SerialFlocPacket received!\r\n");
    printBufferContents(packetBuffer, size);
#endif // DEBUG_ON

    uint8_t pkt_type = *(packetBuffer++); // Remove '$' prefix

    SerialFlocPacket_t* pkt = (SerialFlocPacket_t*)(packetBuffer);

    if (pkt_type == SERIAL_NEST_TO_BURD_TYPE) {
        switch (pkt->header.type) {
            // Broadcast the data received on the serial line
            case SERIAL_BROADCAST_TYPE: // 'B'
            {
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Serial Broadcast Packet Received...\r\n");
            #endif // DEBUG_ON

                SerialBroadcastPacket_t* broadcastPacket = (SerialBroadcastPacket_t* )&pkt->payload;
                broadcast((uint8_t*) broadcastPacket, pkt->header.size);
                break;
            }
            case SERIAL_UNICAST_TYPE:   // 'U'
                // TODO : need to extract dst from packet in order to send packet
                // May not need to implement, depending on networking strategy
                break;
            default:
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Unhandled Serial NeST-to-BuRD packet type!");
                Serial.printf("Prefix [%c]\r\n", (char) pkt->header.type);
            #endif // DEBUG_ON

                return;
        }
    } else {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Unhandled Serial packet type! Prefix [%c]\r\n", pkt_type);
    #endif // DEBUG_ON

    }
}

extern Service nestSerialServiceDesc;

void
nestSerialService(
    void
){
    nestSerialServiceDesc.busy = false;

    while (nest_connection.available() > 0) {
        char nest_char = nest_connection.read();

        // Check for <CR><LF> sequence
        if (nest_char == '\n' && packetBuffer_nest_idx > 0 && packetBuffer_nest[packetBuffer_nest_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_nest[packetBuffer_nest_idx - 1] = 0;

            init_da();

            packet_received_nest(packetBuffer_nest, packetBuffer_nest_idx - 1);

            act_upon();
            
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

        nestSerialServiceDesc.busy = true;
    }
}

void
nestSerial_init(
    void
){
#ifndef DEBUG_ON // !DEBUG_ON
    nest_connection.begin(115200);
    delay(100);
#endif

}
#endif