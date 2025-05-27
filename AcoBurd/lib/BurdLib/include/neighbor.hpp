#pragma once

#include <stdint.h>
#define MAX_NEIGHBORS 10

typedef struct 
Neighbor {
    uint16_t    devAdd = 0xFFFF; // Device address
    uint16_t    range = 0xFFFF; // 0xFFFF means unknown
    uint32_t    lastRanged = 0xFFFF; // 0xFFFF means unknown
} Neighbor;

class 
NeighborManager {
    public: 
        void 
        add_neighbor(
            uint16_t devAdd
        );

        void 
        remove_neighbor(
            uint16_t devAdd
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

        bool  
        rangeTimeout(
            void
        );

        void 
        update_neighbors(
            uint16_t devAdd,
            uint16_t range
        );

        void 
        start_ranging(
            void
        );

    private:
        uint64_t lastUpdateTime = 0;
        const uint64_t updateInterval = (60*60*1000); // 1 hour

        Neighbor neighbors[MAX_NEIGHBORS];
        uint16_t neighbors_size = 0;

        bool
        check_for_neighbors(
            uint16_t dev_add
        );

        int 
        get_top_3(
            Neighbor** rec_neighbors
        );

        static int 
        compare_neighbors(
            const void* a, 
            const void* b
        ){
            Neighbor *neighborA = *(Neighbor **)a;
            Neighbor *neighborB = *(Neighbor **)a;

            if (neighborA->lastRanged > neighborB->lastRanged) return -1;
            if (neighborA->lastRanged < neighborB->lastRanged) return 1;
            return 0;
        }
};

extern NeighborManager neighborManager;

void
neighborService(
    void
);