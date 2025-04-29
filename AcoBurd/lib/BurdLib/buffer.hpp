#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include "floc.hpp"

// NEED THESE TO AVOID CONFLICTS WITH std::min AND std::max
// also need to be before <algorithm> is included
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

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

    private:
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

        std::queue<FlocPacket_t> commandBuffer;
        std::queue<FlocPacket_t> responseBuffer;

        // this is going to be different
        std::queue<FlocPacket_t> retransmissionBuffer;

        std::map<uint8_t, int> ackIDs;
        std::map<uint8_t, int> transmissionCounts;
        const int maxTransmissions = 5;
};

extern FLOCBufferManager flocBuffer;

#endif
