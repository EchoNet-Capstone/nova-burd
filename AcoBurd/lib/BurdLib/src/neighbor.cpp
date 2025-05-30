#include "safe_arduino.hpp"

#include <stdint.h>

#include <nmv3_api.hpp>
#include <floc_buffer.hpp>

#include "activity_period.hpp"
#include "neighbor.hpp"
#include "services.hpp"
#include "utils.hpp"

#define STALE(a) (a >= (60 * 60 * 1000)) // 60 minutes

NeighborManager neighborManager;

bool timeout = false;

// helper functions

bool
NeighborManager::check_for_neighbors(
    uint16_t dev_add
){
    for(uint8_t i = 0; i < neighbors_size; i++) {
        if (neighbors[i].devAdd == dev_add) {

            return true; // found
        }
    }

    return false; // not found
}

int 
NeighborManager::get_top_3(
    void
){
    if (neighbors_size == 0) {
        return 0; // Empty Neighbor List
    }

    int count = 0;

    for (uint8_t i = 0; i < neighbors_size && count < NEIGHBORS_TO_PING; i++) {
        pingList[count++].devAdd = neighbors[i].devAdd;
    }

    return count;
}

void 
NeighborManager::print_neighbors(
    void
){ 
    for (uint8_t i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd == UNKNOWN) {
            continue;
        }

        Serial.printf("Neighbor %02i:\r\n",i);
        Serial.printf("\tdevAdd=%04X\r\n", neighbors[i].devAdd);
        Serial.printf("\tlastRanged=%i\r\n", neighbors[i].lastRanged);
        Serial.printf("\trange=%d\r\n", neighbors[i].range);
    }
}

void 
NeighborManager::add_neighbor(
    uint16_t devAdd
){
    // check for devAdd in the list

    if (check_for_neighbors(devAdd)) {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Neighbor already exists: devAdd=%04X\r\n", ntohs(devAdd));
        Serial.printf("\tUpdating last seen \r\n");
    #endif // DEBUG_ON
        return;
    }

    if (neighbors_size >= MAX_NEIGHBORS){
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Neighbor list full! Not adding neighbor...\r\n");
    #endif // DEBUG_ON

        return;
    }

    if (devAdd > get_device_id()){
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Higher device ID. ");
        Serial.printf("Only care about Neighbors with a lower Device ID. ");
        Serial.printf("Not Adding neighbor....\r\n");
    #endif // DEBUG_ON

        return;
    }

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Adding neighbor: devAdd=%04X at time %i \r\n", devAdd, millis());
#endif // DEBUG_ON

    for (uint8_t i = 0; i < MAX_NEIGHBORS; i++) {
        if ( neighbors[i].devAdd == UNKNOWN ){
            neighbors[i].devAdd = devAdd;
            neighbors[i].lastRanged = millis();
            break;
        }
    }

    neighbors_size++;

    #ifdef DEBUG_ON // DEBUG_ON
        neighborManager.print_neighbors();
    #endif // DEBUG_ON
}

void 
NeighborManager::clear_neighbors(
    void
){
    for (uint8_t i = 0; i < MAX_NEIGHBORS; i++) {
        neighbors[i].devAdd = UNKNOWN;
        neighbors[i].lastRanged = UNKNOWN;
        neighbors[i].range = UNKNOWN;
    }

    neighbors_size = 0;

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Cleared all neighbors\r\n");
#endif // DEBUG_ON
}

void 
NeighborManager::timeout_neighbors(
    void
){
    uint32_t currentTime = millis();

    uint8_t num_timedout = 0;

    for (uint8_t i = 0; i < MAX_NEIGHBORS; i++) {
        if(neighbors[i].devAdd == UNKNOWN){
            continue;
        }

        uint32_t timeDiff = currentTime - neighbors[i].lastRanged;
        
        if ( timeDiff > TIMEOUT_NEIGHBORS) {
            // Neighbor has timed out

        #ifdef DEBUG_ON
            Serial.printf("Neighbor data timed out: devAdd=%d\r\n", neighbors[i].devAdd);
        #endif

            num_timedout++;

            // Mark the slot as unused
            neighbors[i].devAdd = UNKNOWN;
            neighbors[i].range = UNKNOWN;
            neighbors[i].lastRanged = UNKNOWN;
        }
    }

    neighbors_size -= num_timedout;
}

bool
NeighborManager::rangeTimeout(
    void
){
    if ((millis() - neighborManager.lastUpdateTime) > neighborManager.updateInterval) {

        return true;
    }

    return false;
}

// start ranging protocol
void
NeighborManager::start_ranging(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Starting ranging period\r\n");
    print_neighbors();   
#endif // DEBUG_ON

    memset(pingList, 0, sizeof(pingList));

    timeout_neighbors();

    qsort(neighbors, neighbors_size, sizeof(Neighbor), compare_neighbors);

    pingList_size = get_top_3();

    set_activity_state(RANGING);
}

void
NeighborManager::pingHandler(
    void
){
    enum pingState_e : uint8_t{
        SEND_PING,
        WAIT_FOR_RESP
    };

    static pingState_e pingState = SEND_PING;
    static uint8_t current_ping = 0;

    if(current_ping >= pingList_size || neighbors_size == 0){
    #ifdef DEBUG_ON // DEBUG_ON
        if(neighbors_size == 0){
            Serial.printf("No neighbors to range against...\r\n");
        }
        Serial.printf("Ending ranging period...\r\n");
    #endif // DEBUG_ON
        
        pingState = SEND_PING;
        current_ping = 0;

        lastUpdateTime = millis();

        set_activity_state(SENDING);
        return;
    }

    switch(pingState){
        case SEND_PING:{
            if(pingList[current_ping].pings >= MAX_PINGS){
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Coudn't get range data for %04X...\r\n", pingList[current_ping].devAdd);
            #endif // DEBUG_ON

                current_ping++;
                pingState = SEND_PING;
                break;
            }

            uint8_t modemAdd = modemIdFromDidNid(pingList[current_ping].devAdd, get_network_id());

        #ifdef DEBUG_ON // DEBUG_ON
            Serial.printf("Attempting to ping %04X (modem: %03d)...\r\n", pingList[current_ping].devAdd, modemAdd);
        #endif // DEBUG_ON

            pingList[current_ping].pings++;
            ping(modemAdd);

            pingState = WAIT_FOR_RESP;
            timeout = false;

            break;
        }
        case WAIT_FOR_RESP:
            if(pingList[current_ping].dataRecvd){
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Recieved data for %04X...\r\n", pingList[current_ping].devAdd);
            #endif // DEBUG_ON

                current_ping++;
                pingState = SEND_PING;

            } else if (timeout) {
            #ifdef DEBUG_ON // DEBUG_ON
                Serial.printf("Timedout waiting for data from ");
                Serial.printf("%04X. Trying again...\r\n", pingList[current_ping].devAdd);
            #endif // DEBUG_ON

                pingState = SEND_PING;
                timeout = false;

            } else {
                /* Do nothing */
            }

            break;
    }
}

void
NeighborManager::acknowledgeTimeout(
    void
){
    timeout = true;
}

void
NeighborManager::update_neighbor(
    uint8_t modemAdd,
    uint16_t range
){  
    if(get_activity_state() != RANGING){
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Not in ranging period, but ");
        Serial.printf("received range data!\r\n");
    #endif // DEBUG_ON

        /* Do nothing */
        return;
    }

    bool neighbor_found = false;

    uint8_t i = 0;
    for (; i < neighbors_size; i++) {
        if (modemIdFromDidNid(neighbors[i].devAdd, get_network_id()) == modemAdd) {
            neighbors[i].range = range;
            neighbors[i].lastRanged = millis(); // update timestamp
            
            break;
        }
    }

    if (i == neighbors_size){ 
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Received range data for a Neighbor ");
        Serial.printf("not in the neighbor list!\r\n");
    #endif // DEBUG_ON

        return;
    }

    for(i = 0; i < NEIGHBORS_TO_PING; i++){
        if(modemIdFromDidNid(pingList[i].devAdd, get_network_id()) == modemAdd){
            pingList[i].dataRecvd = true;

            return;
        }
    }

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("In ranging period, but recieved data ");
    Serial.printf("for a device we aren't ranging for!\r\n");
#endif // DEBUG_ON
}

// neighbor service itself
extern Service neighborServiceDesc;

void 
neighborService(
    void
){

    neighborServiceDesc.busy = false;

    if (get_activity_state() == RANGING){
        neighborManager.pingHandler();

        neighborServiceDesc.busy = true;
        return;
    }

    // // Check for new neighbors
    if (neighborManager.rangeTimeout()) {
        neighborManager.start_ranging();

        neighborServiceDesc.busy = true;
    }
}
