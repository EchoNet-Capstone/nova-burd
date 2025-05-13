#pragma once

#include <stdint.h>
#define MAX_NEIGHBORS 10

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

        void 
        update_neighbors(
            uint16_t devAdd,
            uint16_t range
        );

    private:
        uint64_t lastUpdateTime = 0;
        const uint64_t updateInterval = (60*60*1000); // 1 hour

        Neighbor neighbors[MAX_NEIGHBORS];

        void 
        ping_recent_neighbors(
            void
        );

        void 
        start_ranging(
            void
        );

        void 
        get_top_3(
            Neighbor *rec_neighbors[3]
        );

        static int 
        compare_recent(
            const void *a, 
            const void *b
        );

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