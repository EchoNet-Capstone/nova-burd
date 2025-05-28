#include "safe_arduino.hpp"

#include <stdint.h>

#include <nmv3_api.hpp>
#include <floc_buffer.hpp>

#include "activity_period.hpp"
#include "neighbor.hpp"
#include "services.hpp"
#include "utils.hpp"

NeighborManager neighborManager;

#define TIMOUT_NEIGHBORS (60 * 60 * 1000 * 6) // 6 hours

#define STALE(a) (a < (60 * 60 * 1000)) // 60 minutes
#define UNKNOWN 0xFFFF
#define SEND_AMOUNT 3

void 
NeighborManager::add_neighbor(
    uint16_t devAdd
){
    // check for devAdd in the list

    if (check_for_neighbors(devAdd)) {
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Neighbor already exists: devAdd=%d\r\n", ntohs(devAdd));
        Serial.printf("\tUpdating last seen \r\n");
    #endif // DEBUG_ON
        return;
    }

    if (neighbors_size > MAX_NEIGHBORS){
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Neighbor list full! Not adding neighbor...\r\n");
    #endif // DEBUG_ON

        return;
    }

    if (devAdd > get_device_id()){
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Higher device ID.");
        Serial.printf("Only care about Neighbors with a lower Device ID.");
        Serial.printf("Not Adding neighbor....\r\n");
    #endif // DEBUG_ON

        return;
    }

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Adding neighbor: devAdd=%d at time %i \r\n", ntohs(devAdd), millis());
#endif // DEBUG_ON

    Neighbor neighbor;

    neighbor.devAdd = devAdd;

    neighbors[neighbors_size] = neighbor;

    neighbors_size += 1;

    #ifdef DEBUG_ON // DEBUG_ON
        neighborManager.print_neighbors();
    #endif // DEBUG_ON

    qsort(neighbors, neighbors_size, sizeof(Neighbor), compare_neighbors);
}

void 
NeighborManager::remove_neighbor(
    uint16_t devAdd
){
    int i = 0;
    for (; i < neighbors_size; i++) {
        if (neighbors[i].devAdd == devAdd) {
            break;
        }
    }

    if (i == neighbors_size){ // Didn't find neighbor
    #ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Didn't find neighbor: devAdd=%d\r\n", devAdd);
    #endif // DEBUG_ON

        return;
    }

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Removing neighbor: devAdd=%d\r\n", ntohs(devAdd));
#endif // DEBUG_ON

    // Mark the slot as unused
    neighbors[i].devAdd = UNKNOWN;
    neighbors[i].lastRanged = UNKNOWN;
    neighbors[i].range = UNKNOWN;

    qsort(neighbors, neighbors_size, sizeof(Neighbor), compare_neighbors);

    neighbors_size -= 1;
}

void 
NeighborManager::print_neighbors(
    void
){ 
    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd == UNKNOWN) {  // assuming UNKOWN = unused slot, we're at the end of the list
            return;
        }

        Serial.printf(
            "Neighbor: devAdd=%d, lastRanged=%i, range=%d\r\n",
            ntohs(neighbors[i].devAdd),
            neighbors[i].lastRanged,
            neighbors[i].range
        );
    }
}

void 
NeighborManager::clear_neighbors(
    void
){
    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        neighbors[i].devAdd = UNKNOWN;
        neighbors[i].lastRanged = UNKNOWN;
        neighbors[i].range = UNKNOWN;
    }

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Cleared all neighbors\r\n");
#endif // DEBUG_ON
}

void 
NeighborManager::timeout_neighbors(
    void
){
    uint32_t currentTime = millis();

    int num_timedout = 0;

    for (int i = 0; i < neighbors_size; i++) {
        if ((currentTime - neighbors[i].lastRanged) > updateInterval) {
        // Neighbor has timed out

        #ifdef DEBUG_ON
            Serial.printf("Neighbor timed out: devAdd=%d\r\n", neighbors[i].devAdd);
        #endif

            num_timedout++;

            // Mark the slot as unused
            neighbors[i].devAdd = UNKNOWN;
            neighbors[i].range = UNKNOWN;
            neighbors[i].lastRanged = UNKNOWN;
        }
    }

    qsort(neighbors, neighbors_size, sizeof(Neighbor), compare_neighbors);

    neighbors_size -= num_timedout;
}

// fix
bool
NeighborManager::rangeTimeout(
    void
){
    if (millis() - neighborManager.lastUpdateTime > neighborManager.updateInterval) {
        neighborManager.lastUpdateTime = millis();

        #ifdef DEBUG_ON // DEBUG_ON
            Serial.printf("Starting ranging period\r\n");
            neighborManager.print_neighbors();   
        #endif // DEBUG_ON

        return true;
    }

    return false;
}

// start ranging protocol
void
NeighborManager::start_ranging(
    void
){
    Neighbor* rec_neighbors[SEND_AMOUNT] = {0};
    
    get_top_3(rec_neighbors);

    for (int i = 0; i < SEND_AMOUNT; i++) {

        if (rec_neighbors[i] != NULL) {            
        #ifdef DEBUG_ON // DEBUG_ON
            Serial.printf("Requesting range from neighbor: devAdd=%d\r\n", ntohs(neighbors[i].devAdd));
        #endif

            flocBuffer.addToPingList(i, rec_neighbors[i]->devAdd);
        }
    }
}


void
NeighborManager::update_neighbors(
    uint16_t devAdd,
    uint16_t range
){
    for (int i = 0; i < neighbors_size; i++) {
        if (neighbors[i].devAdd == devAdd) {
            neighbors[i].range = range;
            neighbors[i].lastRanged = millis(); // update timestamp
            break;
        }
    }

    qsort(neighbors, neighbors_size, sizeof(Neighbor), compare_neighbors);
}

// helper functions

bool
NeighborManager::check_for_neighbors(
    uint16_t dev_add
){
    for (int i = 0; i < neighbors_size; i++) {
        if (neighbors[i].devAdd == dev_add) {
            return true; // found
        }
    }

    return false; // not found
}

int 
NeighborManager::get_top_3(
    Neighbor** rec_neighbors
){
    if (neighbors_size == 0) {
        return 0; // Empty Neighbor List
    }

    int count = 0;

    for (int i = 0; i < neighbors_size && count < SEND_AMOUNT; i++) {
        rec_neighbors[count++] = &neighbors[i];
    }

    return count;
}

// neighbor service itself

extern Service neighborServiceDesc;

void 
neighborService(
    void
){

    neighborServiceDesc.busy = false;

    // // Check for new neighbors
    if (neighborManager.rangeTimeout()) {

        neighborManager.start_ranging();

        neighborServiceDesc.busy = true;
    }
}
