#pragma once

#include <stdint.h>
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define MAX_NEIGHBORS 10


typedef struct Neighbor {
    uint16_t    devAdd; // Device address
    uint8_t     modAdd; // Modem address
    uint64_t    lastSeen;
    uint16_t    range = 0xFFFF; // 0xFFFF means unknown
} Neighbor;




class NeighborManager {
    public: 
        void add_neighbor(uint16_t devAdd, uint8_t modAdd);
        void remove_neighbor(uint16_t devAdd);
        void update_neighbor_range(uint16_t devAdd, uint16_t range);
        void print_neighbors();
        void clear_neighbors();
        void timeout_neighbors();
        int  rangeTimeout();
    private:
        int check_for_neighbors(uint16_t dev_add);
        uint64_t lastUpdateTime = 0;
        const uint64_t updateInterval = 300000; // 300 seconds

        void ping_recent_neighbors(void);

        Neighbor neighbors[MAX_NEIGHBORS];

};


extern NeighborManager neighborManager;

void neighborService(void);