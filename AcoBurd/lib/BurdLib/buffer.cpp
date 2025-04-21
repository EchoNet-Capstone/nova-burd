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
  

#include "buffer.hpp"
#include "floc.hpp"

#define DEBUG 1



void FLOCBufferManager::addPacket(const FlocPacket_t& packet) {
    if(packet.header.type == FLOC_COMMAND_TYPE) {
        commandBuffer.push(packet);
    } else if (packet.header.type == FLOC_RESPONSE_TYPE) {
        responseBuffer.push(packet);
    } else if (packet.header.type == FLOC_ACK_TYPE) {
        retransmissionBuffer.push(packet);
    } else {
        printf("Invalid packet type for command buffer\n");
    }
}

// blocking check call
int FLOCBufferManager::checkqueueStatus(){
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

int FLOCBufferManager::retransmission_handler() {
    FlocPacket_t packet = retransmissionBuffer.front();
    // send packet
    return 0;
}

int FLOCBufferManager::response_handler() {
    FlocPacket_t packet = responseBuffer.front();
    // send packet
    return 0;
}

int FLOCBufferManager::command_handler() {
    FlocPacket_t packet = commandBuffer.front();
    if (packet.header.ttl == 0) {
        commandBuffer.pop();
        return 0;
    }
    // send packet
    return 0;
}