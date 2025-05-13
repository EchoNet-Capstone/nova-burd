
#include "neighbor.hpp"
#include "safe_arduino.hpp"
#include <stdint.h>
#include <utils.hpp>
#include <globals.hpp>
#include <device_actions.hpp>
#include "services.hpp"
#include "activity_period.hpp"
#include "stdlib.h"
#include "nmv3_api.hpp"
#include "buffer.hpp"

// add to neighbor list and pupulate data

NeighborManager neighborManager;

#define TIMOUT_NEIGHBORS (60 * 60 * 1000 * 6) // 6 hours

#define STALE(a) (a < (60 * 60 * 1000)) // 60 minutes
#define UNKNOWN 0xFFFF
#define SEND_AMOUNT 3


void 
NeighborManager::add_neighbor(
    uint16_t devAdd, 
    uint8_t modAdd
) {
    // check for devAdd in the list

    if (check_for_neighbors(devAdd)) {
#ifdef DEBUG_ON // DEBUG_ON
        Serial.printf("Neighbor already exists: devAdd=%d\n", devAdd);
#endif // DEBUG_ON

        return;
    }

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Adding neighbor: devAdd=%d, modAdd=%d\n", devAdd, modAdd);
#endif // DEBUG_ON


    Neighbor neighbor;
    neighbor.devAdd = devAdd;
    neighbor.modAdd = modAdd;
    neighbor.lastSeen = millis();
    neighbors[devAdd] = neighbor;
}


void 
NeighborManager::remove_neighbor(
    uint16_t devAdd
) {
    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd == devAdd) {
#ifdef DEBUG_ON // DEBUG_ON
            Serial.printf("Removing neighbor: devAdd=%d\n", devAdd);
#endif // DEBUG_ON
            // Mark the slot as unused
            memset(&neighbors[i], 0, sizeof(Neighbor));
        }
    }
}

void 
NeighborManager::update_neighbor_range(
    uint16_t devAdd, uint16_t range
){
    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd == devAdd) {
            neighbors[i].range = range;
            neighbors[i].lastSeen = millis(); // update timestamp
        }
    }
}


void NeighborManager::print_neighbors(

) {
    for (int i = 0; i < 10; i++) {
        if (neighbors[i].devAdd != 0xFFFF) {  // assuming 0xFFFF = unused slot
            Serial.printf(
                "Neighbor: devAdd=%d, modAdd=%d, lastSeen=%lu, range=%d\n",
                neighbors[i].devAdd,
                neighbors[i].modAdd,
                neighbors[i].lastSeen,
                neighbors[i].range
            );
        }
    }
}

void 
NeighborManager::clear_neighbors(
    void
) {
    for (int i = 0; i < 10; i++) {
        neighbors[i].devAdd = UNKNOWN;
        neighbors[i].modAdd = 0;
        neighbors[i].lastSeen = 0;
        neighbors[i].range = UNKNOWN;
    }
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Cleared all neighbors\n");
#endif // DEBUG_ON
}

void 
NeighborManager::timeout_neighbors(
    void
) {
    uint64_t currentTime = millis();
    for (int i = 0; i < 10; i++) {
        if (neighbors[i].devAdd != UNKNOWN && (currentTime - neighbors[i].lastSeen > updateInterval)) {
            // Neighbor has timed out

#ifdef DEBUG_ON
            Serial.printf("Neighbor timed out: devAdd=%d\n", neighbors[i].devAdd);
#endif

            // Mark the slot as unused
            neighbors[i].devAdd = UNKNOWN;
            neighbors[i].range = UNKNOWN;
            neighbors[i].modAdd = 0;
            neighbors[i].lastSeen = 0;
        }
    }
}


int
NeighborManager::check_for_neighbors(
    uint16_t dev_add
) {
    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd == dev_add) {
            neighbors[i].lastSeen = millis(); // update timestamp
            return 1; // found
        }
    }
    return 0; // not found
}


extern Service neighborServiceDesc;

void 
neighborService(
    void
) {

    neighborServiceDesc.busy = false;

    // Check for new neighbors
    if (neighborManager.rangeTimeout()) {

        neighborServiceDesc.busy = true;


    }

}







// fix
int
NeighborManager::rangeTimeout(
    void
) {
    if (millis() - neighborManager.lastUpdateTime > neighborManager.updateInterval) {
        neighborManager.lastUpdateTime = millis();
        neighborManager.print_neighbors();

#ifdef DEBUG_ON // DEBUG_ON
            Serial.printf("Starting ranging period\n");   
#endif // DEBUG_ON
        return 1;
    }
    return 0;
}

// start ranging protocol
void
NeighborManager::start_ranging(
    void
) {
    Neighbor *rec_neighbors[SEND_AMOUNT];
    get_top_3(rec_neighbors);

    for (int i = 0; i < SEND_AMOUNT; i++) {

        if (rec_neighbors[i] != NULL) {            
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Requesting range from neighbor: devAdd=%d, modAdd=%d\n",
        neighbors[i].devAdd,
        neighbors[i].modAdd
    );
#endif
            flocBuffer.add_pinglist(i, rec_neighbors[i]->devAdd, rec_neighbors[i]->modAdd);

        }
    }

}

// ikykyk
void
NeighborManager::update_neighbors(
    uint16_t devAdd,
    uint16_t range
) {
    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd == devAdd) {
            neighbors[i].range = range;
            neighbors[i].lastSeen = millis(); // update timestamp
        }
    }
}

// helper functions

// get most recent 3 
void 
NeighborManager::get_top_3(
    Neighbor *rec_neighbors[SEND_AMOUNT]
) {
    Neighbor *temp[MAX_NEIGHBORS];
    int count = 0;

    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd != UNKNOWN) {
            temp[count++] = &neighbors[i];

        }
    }

    qsort(temp, count, sizeof(Neighbor *), compare_recent);

    for (int i = 0; i < SEND_AMOUNT && i < count; i++) {
        rec_neighbors[i] = temp[i];
    }
}

int 
NeighborManager::compare_recent(
    const void *a, 
    const void *b
) {
    Neighbor *neighborA = *(Neighbor **)a;
    Neighbor *neighborB = *(Neighbor **)b;

    if (neighborA->lastSeen > neighborB->lastSeen) return -1;
    if (neighborA->lastSeen < neighborB->lastSeen) return 1;
    return 0;
}

