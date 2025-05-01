#pragma once

#include <stdint.h>
#include <map>

typedef struct 
Neighbor {
    uint16_t    devAdd; // Device address
    uint8_t     modAdd; // Modem address
    uint64_t    lastSeen;
    uint16_t    range = 0xFFFF; // 0xFFFF means unknown
} Neighbor;

class 
NeighborManager {
    public: 
        void 
        add_neighbor(
            uint16_t devAdd, 
            uint8_t modAdd
        );

        void 
        remove_neighbor(
            uint16_t devAdd
        );

        void 
        update_neighbor_range(
            uint16_t devAdd, 
            uint16_t range
        );

        void 
        print_neighbors(
            void
        );

        void 
        clear_neighbors(
            void
        );

        void 
        timeout_neighbors(
            void
        );

        int  
        rangeTimeout(
            void
        );
    private:
        uint64_t lastUpdateTime = 0;
        const uint64_t updateInterval = 300000; // 300 seconds

        std::map<uint16_t, Neighbor> neighbors;

        int 
        check_for_neighbors(
            uint16_t dev_add
        );

        void
        ping_recent_neighbors(
            void
        );
};

extern NeighborManager neighborManager;

void
neighborService(
    void
);