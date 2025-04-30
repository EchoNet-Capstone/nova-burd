#pragma once

#include <stdint.h>

#include <map>

typedef struct Neighbor {
    uint16_t    devAdd; // Device address
    uint8_t     modAdd; // Modem address
    uint64_t    lastSeen;
    uint16_t    range = 0xFFFF; // 0xFFFF means unknown
} Neighbor;



class NeighborManager {
    public: 
        void add_neighbor(uint16_t devAdd, uint8_t modAdd);
        void update_neighbor(uint16_t devAdd, uint8_t modAdd);
        void remove_neighbor(uint16_t devAdd);
        void update_neighbor_range(uint16_t devAdd, uint16_t range);
        void update_neighbor_last_seen(uint16_t devAdd);
        void print_neighbors();
        void clear_neighbors();
        void update_neighbors();
    private:
        void check_for_neighbors(void);
        uint64_t lastUpdateTime = 0;
        const uint64_t updateInterval = 10000; // 10 seconds

        std::map<uint16_t, Neighbor> neighbors;
};


extern NeighborManager neighborManager;