#include "safe_arduino.hpp"

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>
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
    if (neighbors.find(devAdd) != neighbors.end()) {
        neighbors.erase(devAdd);
    }
}

void 
NeighborManager::update_neighbor_range(
    uint16_t devAdd,
    uint16_t range
){
    if (neighbors.find(devAdd) != neighbors.end()) {
        neighbors[devAdd].range = range;
    }
}

void
NeighborManager::print_neighbors(
    void
){
    for (const auto& pair : neighbors) {
        const Neighbor& neighbor = pair.second;
        Serial.printf("Neighbor: devAdd=%d, modAdd=%d, lastSeen=%lu, range=%d\n", neighbor.devAdd, neighbor.modAdd, neighbor.lastSeen, neighbor.range);
    }
}

void 
NeighborManager::clear_neighbors(
    void
){
    neighbors.clear();
}

void 
NeighborManager::timeout_neighbors(
    void
){
    uint64_t currentTime = millis();
    for (auto it = neighbors.begin(); it != neighbors.end();) {
        if (currentTime - it->second.lastSeen > TIMEOUT) { // 30 seconds

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Neighbor timed out: devAdd=%d\n", it->first); 
#endif // DEBUG_ON

            it = neighbors.erase(it);
        } else {
            ++it;
        }
    }
}

int
NeighborManager::check_for_neighbors(
    uint16_t dev_add
){
    if (neighbors.find(dev_add) != neighbors.end()) {
        neighbors[dev_add].lastSeen = millis();
        return 1;
    }
    return 0;
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

extern Service neighborServiceDesc;

void 
neighborService(
    void
){

    neighborServiceDesc.busy = false;

    // Check for new neighbors
    if (neighborManager.rangeTimeout()) {

        neighborServiceDesc.busy = true;
    }
}