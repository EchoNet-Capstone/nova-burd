#pragma once

#include <stdint.h>
#define MAX_NEIGHBORS 10
#define TIMEOUT_NEIGHBORS (60 * 60 * 1000 * 6) // 6 hours

#define UNKNOWN 0xFFFF

#define NEIGHBORS_TO_PING 3
#define MAX_PINGS 5

typedef struct 
Neighbor {
    uint16_t    devAdd = UNKNOWN; // Device address
    uint16_t    range = UNKNOWN;
    uint32_t    lastRanged = 0; 
} Neighbor;

typedef struct{
    uint16_t    devAdd;
    uint8_t     pings;
    bool        dataRecvd;
} PingEntry_t;

class 
NeighborManager {
    public: 
        void 
        add_neighbor(
            uint16_t devAdd
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
        start_ranging(
            void
        );

        void
        pingHandler(
            void
        );

        void 
        acknowledgeTimeout(
            void
        );

        void 
        update_neighbor(
            uint8_t modemAdd,
            uint16_t range
        );

    private:
        uint64_t lastUpdateTime = 0;

    #ifdef DEBUG_ON // DEBUG_ON
        const uint64_t updateInterval = (60*1000); // 30 sec test
    #else
        const uint64_t updateInterval = (5*60*1000); // 5 minutes
    #endif

        Neighbor neighbors[MAX_NEIGHBORS];
        uint16_t neighbors_size = 0;

        PingEntry_t pingList[NEIGHBORS_TO_PING] = {0};
        uint8_t pingList_size = 0;

        void 
        print_neighbors(
            void
        );

        bool
        check_for_neighbors(
            uint16_t dev_add
        );

        int 
        get_top_3(
            void
        );

        static int 
        compare_neighbors(
            const void* a, 
            const void* b
        ){
            Neighbor *neighborA = (Neighbor*)a;
            Neighbor *neighborB = (Neighbor*)b;

            // Empty entries (devAdd == 0xFFFF) go to the end (4th priority)
            if (neighborA->devAdd == 0xFFFF && neighborB->devAdd == 0xFFFF) return 0;
            if (neighborA->devAdd == 0xFFFF) return 1;  // A empty, B comes first
            if (neighborB->devAdd == 0xFFFF) return -1; // B empty, A comes first

            // Unknown entries (range == 0xFFFF) have highest priority (1st)
            bool aUnknown = (neighborA->range == 0xFFFF);
            bool bUnknown = (neighborB->range == 0xFFFF);
            
            if (aUnknown && bUnknown) {
                // Both unknown - sort by device address (lower first)
                if (neighborA->devAdd < neighborB->devAdd) return -1;
                if (neighborA->devAdd > neighborB->devAdd) return 1;
                return 0;
            }
            if (aUnknown) return -1;  // A unknown, comes first
            if (bUnknown) return 1;   // B unknown, comes first
            
            // Both have been ranged - sort by lastRanged (older first)
            // This puts stale entries (2nd priority) before known entries (3rd priority)
            if (neighborA->lastRanged < neighborB->lastRanged) return -1;
            if (neighborA->lastRanged > neighborB->lastRanged) return 1;
            return 0;
        };
};

extern NeighborManager neighborManager;

void
neighborService(
    void
);