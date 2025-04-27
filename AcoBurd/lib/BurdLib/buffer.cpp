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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <optional>
#include <map>
#include <Arduino.h>
  
#include <globals.hpp>
#include "buffer.hpp"
#include <nmv3_api.hpp>
#include "activityperiod.hpp"

#define DEBUG 1



void FLOCBufferManager::addPacket(const FlocPacket_t& packet, int retrans) {
    // identify if the packet is a retransmission
    if (retrans) {
        retransmissionBuffer.push(packet);
        if (DEBUG) printf("Packet added to retransmission buffer\n");
        return;
    }

    // if not a retransmission, check the type of the packet
    if(packet.header.type == FLOC_COMMAND_TYPE) {
        commandBuffer.push(packet);
    } else if (packet.header.type == FLOC_RESPONSE_TYPE || packet.header.type == FLOC_ERROR_TYPE) {
        responseBuffer.push(packet);
    } else {
        printf("Invalid packet type for command buffer\n");
    }
}

// blocking check call
int FLOCBufferManager::queuehandler(){
    if(!retransmissionBuffer.empty()) {
        if (DEBUG) printf("Retransmission buffer is not empty\n");
        retransmission_handler();
        return 1;
    } else if (!responseBuffer.empty()) {
        if (DEBUG) printf("Response buffer is not empty\n");
        response_handler();
        return 2;
    } else if (!commandBuffer.empty()) {
        if (DEBUG) printf("Command buffer is not empty\n");
        command_handler();
        return 3;
    } else {
        return 0;
    }
}

// check if buffer is empty
int FLOCBufferManager::checkqueueStatus(){
    if(!retransmissionBuffer.empty()) {
        if (DEBUG) printf("Retransmission buffer is not empty\n");
        return 1;
    } else if (!responseBuffer.empty()) {
        if (DEBUG) printf("Response buffer is not empty\n");
        return 2;
    } else if (!commandBuffer.empty()) {
        if (DEBUG) printf("Command buffer is not empty\n");
        return 3;
    } else {
        return 0;
    }
}

// retransmit and remove from vector
int FLOCBufferManager::retransmission_handler() {
    FlocPacket_t packet = retransmissionBuffer.front();

    uint16_t packet_id = packet.header.pid;

    // if message is an ack, remove from buffer
    if (checkackID(packet_id)) {
        if (DEBUG) printf("Ack ID %d found and removed\n", packet_id);
        commandBuffer.pop(); // Remove from buffer
        return 1;
    }

    // send packet
    // FIX THE SIZE ASPECT
    broadcast(MODEM_SERIAL_CONNECTION, (char*)&packet, DATA_PACKET_ACTUAL_SIZE(&packet));

    retransmissionBuffer.pop(); // Remove from buffer
    return 0;
}

int FLOCBufferManager::response_handler() {
    FlocPacket_t packet = responseBuffer.front();

    uint16_t packet_id = packet.header.pid;

    // if message is an ack, remove from buffer
    if (checkackID(packet_id)) {
        if (DEBUG) printf("Ack ID %d found and removed\n", packet_id);
        commandBuffer.pop(); // Remove from buffer
        return 1;
    }
    // send packet
    broadcast(MODEM_SERIAL_CONNECTION, (char*)&packet, RESPONSE_PACKET_ACTUAL_SIZE(&packet));
    responseBuffer.pop(); // Remove from buffer
    return 0;
}

int FLOCBufferManager::command_handler() {
    // copy the packet from the front of the queue
    FlocPacket_t packet = commandBuffer.front();
    
    uint8_t packet_id = packet.header.pid;

    // if message is an ack, remove from buffer
    if (checkackID(packet_id)) {
        if (DEBUG) printf("Ack ID %d found and removed\n", packet_id);
        commandBuffer.pop(); // Remove from buffer
        return 1;
    }

    // Check if the packet ID exists in the map, if not initialize it
    if (transmissionCounts.find(packet_id) == transmissionCounts.end()) {
        transmissionCounts[packet_id] = 0; // Initialize count for this packet ID
    }

    // Check if the packet has been transmitted the maximum number of times
    if(transmissionCounts[packet_id] >= maxTransmissions) {
        if (DEBUG) printf("Max transmissions reached for packet ID %d\n", packet_id);
        commandBuffer.pop(); // Remove from buffer
        transmissionCounts.erase(packet_id); // Remove from map

        floc_error_send(1,  packet.header.dest_addr, pack); // Send error packet
        return 0;
    }

    transmissionCounts[packet_id]++; // Increment transmission count for this packet ID

    broadcast(MODEM_SERIAL_CONNECTION, (char*)&packet, COMMAND_PACKET_ACTUAL_SIZE(&packet));
    // send packet
    return 0;
}

// list of ackIDs
void FLOCBufferManager::add_ackID(uint8_t ackID) {
    ackIDs[ackID] = 1;
    if (DEBUG) printf("Ack ID %d added\n", ackID);
}

int FLOCBufferManager::checkackID(uint8_t ackID) {
    if (ackIDs.find(ackID) != ackIDs.end()) {
        ackIDs.erase(ackID);
        if (DEBUG) printf("Ack ID %d found and removed\n", ackID);
        return 1;
    } else {
        return 0;
    }
}