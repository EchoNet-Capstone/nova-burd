#pragma once

#include <stdint.h>

// NEED THESE TO AVOID CONFLICTS WITH std::min AND std::max
// also need to be before <algorithm> is included
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <floc.hpp>

#include <stddef.h>
#include <string.h>

#include <optional>
#include <map>
#include <queue>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>
#include <cstdint>

struct ping_device {
    uint16_t devAdd;
    uint8_t modAdd;
    uint8_t pingCount;
};

class 
FLOCBufferManager {
    public:
        void
        addPacket(
            const FlocPacket_t& packet, int retrans = 0
        );

        int
        checkqueueStatus(
            void
        );

        int
        queuehandler(
            void
        );

        void
        add_ackID(
            uint8_t ackID
        );

        void
        add_pinglist(
            uint8_t index,
            uint16_t devAdd,
            uint8_t modAdd
        );

    private:

        int
        ping_handler(
            void
        );

        int
        retransmission_handler(
            void
        );

        int
        response_handler(
            void
        );

        int
        command_handler(
            void
        );

        int
        checkackID(
            uint8_t ackID
        );

        bool
        check_pinglist(
            void
        );


        ping_device pingDevice[3];;

        std::queue<FlocPacket_t> commandBuffer;
        std::queue<FlocPacket_t> responseBuffer;

        // this is going to be different
        std::queue<FlocPacket_t> retransmissionBuffer;

        std::map<uint8_t, int> ackIDs;
        std::map<uint8_t, int> transmissionCounts;
        const int maxTransmissions = 5;
};



extern FLOCBufferManager flocBuffer;

void
bufferService(
    void
);