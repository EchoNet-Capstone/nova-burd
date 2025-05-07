#include "safe_arduino.hpp"
#include <stdint.h>

#include "device_actions.hpp"
#include "globals.hpp"
#include "neighbor.hpp"
#include "services.hpp"
#include "utils.hpp"

// add to neighbor list and pupulate data

#define TIMEOUT 100000000 // 1000 seconds?

void 
NeighborManager::add_neighbor(
    uint16_t devAdd,
    uint8_t modAdd
){
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
){
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
    uint16_t devAdd,
    uint16_t range
){
    for (int i = 0; i < MAX_NEIGHBORS; i++) {
        if (neighbors[i].devAdd == devAdd) {
            neighbors[i].range = range;
            neighbors[i].lastSeen = millis(); // update timestamp
        }
    }
}


void 
NeighborManager::print_neighbors(
    void
){ 
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
){
    memset(neighbors, 0, sizeof(neighbors));
}

void 
NeighborManager::timeout_neighbors(
    void
){
    uint64_t currentTime = millis();
    for (int i = 0; i < 10; i++) {
        if (neighbors[i].devAdd != 0xFFFF && (currentTime - neighbors[i].lastSeen > TIMEOUT)) {

        #ifdef DEBUG_ON
            Serial.printf("Neighbor timed out: devAdd=%d\n", neighbors[i].devAdd);
        #endif

            // Mark the slot as unused
            neighbors[i].devAdd = 0xFFFF;
            neighbors[i].range = 0xFFFF;
            neighbors[i].modAdd = 0;
            neighbors[i].lastSeen = 0;
        }
    }
}


int
NeighborManager::check_for_neighbors(
    uint16_t dev_add
){
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

    // // Check for new neighbors
    // if (neighborManager.rangeTimeout()) {

    //     neighborServiceDesc.busy = true;


    // }

}


// void 
// NeighborManager::ping_recent_neighbors(

// ) {
//     // Step 1: Copy the neighbors into a vector
//     std::vector<Neighbor> neighborList;
//     for (const auto& pair : neighbors) {
//         neighborList.push_back(pair.second);
//     }

//     // Step 2: Sort by lastSeen descending
//     std::sort(neighborList.begin(), neighborList.end(), 
//         [](const Neighbor& a, const Neighbor& b) {
//             return a.lastSeen > b.lastSeen;
//         });

//     // Step 3: Ping the top 5 (or fewer if not enough)
//     int count = 0;
//     for (const auto& neighbor : neighborList) {
//         if (count >= 5) break;
//         std::cout << "Pinging Neighbor: devAdd=" << neighbor.devAdd
//                   << ", modAdd=" << (int)neighbor.modAdd << std::endl;

//         // Call your actual ping logic here
//         // ping(neighbor.devAdd, neighbor.modAdd);

//         ++count;
//     }
// }

// int
// NeighborManager::rangeTimeout(
//     void
// ) {
//     if (millis() - neighborManager.lastUpdateTime > neighborManager.updateInterval) {
//         neighborManager.lastUpdateTime = millis();
//     #ifdef DEBUG_ON // DEBUG_ON
//         neighborManager.print_neighbors();
//     #endif // DEBUG_ON
//     #ifdef DEBUG_ON // DEBUG_ON
//         Serial.printf("Starting ranging period\n");   
//     #endif // DEBUG_ON
//         return 1;
//     }
//     return 0;
// }