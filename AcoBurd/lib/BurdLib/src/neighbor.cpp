
#include "neighbor.hpp"
#include "safe_arduino.hpp"
#include <map>
#include <stdint.h>
#include <utils.hpp>
#include <globals.hpp>
#include <device_actions.hpp>

// add to neighbor list and pupulate data


void NeighborManager::add_neighbor(uint16_t devAdd, uint8_t modAdd) {
    Neighbor neighbor;
    neighbor.devAdd = devAdd;
    neighbor.modAdd = modAdd;
    neighbor.lastSeen = millis();
    neighbors[devAdd] = neighbor;
}


void NeighborManager::update_neighbor(uint16_t devAdd, uint8_t modAdd) {
    if (neighbors.find(devAdd) != neighbors.end()) {
        neighbors[devAdd].modAdd = modAdd;
        neighbors[devAdd].lastSeen = millis();
    } else {
        add_neighbor(devAdd, modAdd);
    }
}

void NeighborManager::remove_neighbor(uint16_t devAdd) {
    if (neighbors.find(devAdd) != neighbors.end()) {
        neighbors.erase(devAdd);
    }
}

void NeighborManager::update_neighbor_range(uint16_t devAdd, uint16_t range) {
    if (neighbors.find(devAdd) != neighbors.end()) {
        neighbors[devAdd].range = range;
    }
}

void NeighborManager::update_neighbor_last_seen(uint16_t devAdd) {
    if (neighbors.find(devAdd) != neighbors.end()) {
        neighbors[devAdd].lastSeen = millis();
    }
}

void NeighborManager::print_neighbors() {
    for (const auto& pair : neighbors) {
        const Neighbor& neighbor = pair.second;
        Serial.printf("Neighbor: devAdd=%d, modAdd=%d, lastSeen=%lu, range=%d\n", neighbor.devAdd, neighbor.modAdd, neighbor.lastSeen, neighbor.range);
    }
}

void NeighborManager::clear_neighbors() {
    neighbors.clear();
}

void NeighborManager::update_neighbors() {
    uint64_t currentTime = millis();
    for (auto it = neighbors.begin(); it != neighbors.end();) {
        if (currentTime - it->second.lastSeen > 30000) { // 30 seconds
            it = neighbors.erase(it);
        } else {
            ++it;
        }
    }
}