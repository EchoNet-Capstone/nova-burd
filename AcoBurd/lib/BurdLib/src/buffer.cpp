/* This is going to be the buffer protocol file.
 *
 * all use FIFO
 *
 * Retransmission buffer
 * - priority 1
 * Response buffer
 * - priority 2
 * Command buffer
 *  - priority 3
 *  - 5 max transmissions
 *  - if ack rm from buffer
 *  - if no ack after 5 transmissions, rm from buffer
 */

#include "safe_arduino.hpp"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <optional>
#include <map>

#include <nmv3_api.hpp>

#include "globals.hpp"
#include "buffer.hpp"
#include "activity_period.hpp"
#include "services.hpp"

FLOCBufferManager flocBuffer;

void
FLOCBufferManager::addPacket(
    const FlocPacket_t& packet,
    int retrans
){
    // identify if the packet is a retransmission
    if (retrans) {
        retransmissionBuffer.push(packet);
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Packet added to retransmission buffer\n");
    #endif // DEBUG_ON

        return;
    }

    // if not a retransmission, check the type of the packet
    if(packet.header.type == FLOC_COMMAND_TYPE) {
        commandBuffer.push(packet);
    } else if (packet.header.type == FLOC_RESPONSE_TYPE) {
        responseBuffer.push(packet);
    } else {
        printf("Invalid packet type for command buffer\n");
    }
}

// blocking check call
int
FLOCBufferManager::queuehandler(
    void
){
    if(!retransmissionBuffer.empty()) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Retransmission buffer is not empty\n");
    #endif // DEBUG_ON

        retransmission_handler();
        return 1;
    } else if (!responseBuffer.empty()) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Response buffer is not empty\n");
    #endif // DEBUG_ON

        response_handler();
        return 2;
    } else if (!commandBuffer.empty()) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Command buffer is not empty\n");
    #endif // DEBUG_ON

        command_handler();
        return 3;
    } else {
        return 0;
    }
}

// check if buffer is empty
int
FLOCBufferManager::checkqueueStatus(
    void
){
    if(!retransmissionBuffer.empty()) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Retransmission buffer is not empty\n");
    #endif // DEBUG_ON

        return 1;
    } else if (!responseBuffer.empty()) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Response buffer is not empty\n");
    #endif // DEBUG_ON

        return 2;
    } else if (!commandBuffer.empty()) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Command buffer is not empty\n");
    #endif // DEBUG_ON

        return 3;
    } else {
        return 0;
    }
}

int
FLOCBufferManager::ping_handler(
    void
){
    for (int i = 0; i < 3; i++) {
        ping_device& dev = pingDevice[i]; // Reference the real item

        if (checkackID(dev.devAdd)) {
            memset(&dev, 0, sizeof(dev));
#ifdef DEBUG_ON
            printf("Ping ID %d found and removed\n", dev.devAdd);
#endif
            return 1; // No need to ping if ACK received
        }

        if (dev.pingCount < maxTransmissions) {
            dev.pingCount++;
            ping(dev.devAdd, dev.modAdd);
        }
    }

    return 0;
}
// retransmit and remove from vector
int
FLOCBufferManager::retransmission_handler(
    void
){
    FlocPacket_t packet = retransmissionBuffer.front();

    uint16_t packet_id = packet.header.pid;

    // if message is an ack, remove from buffer
    if (checkackID(packet_id)) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Ack ID %d found and removed\n", packet_id);
    #endif // DEBUG_ON

        commandBuffer.pop(); // Remove from buffer
        return 1;
    }

    // send packet
    // FIX THE SIZE ASPECT
    broadcast(MODEM_SERIAL_CONNECTION, (char*)&packet, DATA_PACKET_ACTUAL_SIZE(&packet));

    retransmissionBuffer.pop(); // Remove from buffer
    return 0;
}

int
FLOCBufferManager::response_handler(
    void
){
    FlocPacket_t packet = responseBuffer.front();

    uint16_t packet_id = packet.header.pid;

    // if message is an ack, remove from buffer
    if (checkackID(packet_id)) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Ack ID %d found and removed\n", packet_id);
    #endif // DEBUG_ON

        commandBuffer.pop(); // Remove from buffer
        return 1;
    }
    // send packet
    broadcast(MODEM_SERIAL_CONNECTION, (char*)&packet, RESPONSE_PACKET_ACTUAL_SIZE(&packet));
    responseBuffer.pop(); // Remove from buffer
    return 0;
}

int
FLOCBufferManager::command_handler(
    void
){
    // copy the packet from the front of the queue
    FlocPacket_t packet = commandBuffer.front();

    uint8_t packet_id = packet.header.pid;

    // if message is an ack, remove from buffer
    if (checkackID(packet_id)) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Ack ID %d found and removed\n", packet_id);
    #endif // DEBUG_ON

        commandBuffer.pop(); // Remove from buffer
        return 1;
    }

    // Check if the packet ID exists in the map, if not initialize it
    if (transmissionCounts.find(packet_id) == transmissionCounts.end()) {
        transmissionCounts[packet_id] = 0; // Initialize count for this packet ID
    }

    // Check if the packet has been transmitted the maximum number of times
    if(transmissionCounts[packet_id] >= maxTransmissions) {
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Max transmissions reached for packet ID %d\n", packet_id);
    #endif // DEBUG_ON

        commandBuffer.pop(); // Remove from buffer
        transmissionCounts.erase(packet_id); // Remove from map

        floc_error_send(1, packet_id, packet.header.src_addr); // Send error packet
        return 0;
    }

    transmissionCounts[packet_id]++; // Increment transmission count for this packet ID

    broadcast(MODEM_SERIAL_CONNECTION, (char*)&packet, COMMAND_PACKET_ACTUAL_SIZE(&packet));
    // send packet
    return 0;
}

// list of ackIDs
void
FLOCBufferManager::add_ackID(
    uint8_t ackID
){
    ackIDs[ackID] = 1;
#ifdef DEBUG_ON // DEBUG_ON
    printf("Ack ID %d added\n", ackID);
#endif // DEBUG_ON

}

int
FLOCBufferManager::checkackID(
    uint8_t ackID
){
    if (ackIDs.find(ackID) != ackIDs.end()) {
        ackIDs.erase(ackID);
    #ifdef DEBUG_ON // DEBUG_ON
        printf("Ack ID %d found and removed\n", ackID);
    #endif // DEBUG_ON

        return 1;
    } else {
        return 0;
    }
}

void
FLOCBufferManager::add_pinglist(
    uint8_t index,
    uint16_t devAdd,
    uint8_t modAdd
){
    pingDevice[index].devAdd = devAdd;
    pingDevice[index].modAdd = modAdd;
    pingDevice[index].pingCount = 0;
}

extern Service bufferServiceDesc;

void
bufferService(
    void
){
    bufferServiceDesc.busy = false;

    if (is_activity_period_open()) {
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

            bufferServiceDesc.busy = true;
        }
    }
}

