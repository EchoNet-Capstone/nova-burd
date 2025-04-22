#pragma once

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
#include <algorithm>  // <-- This is important for fill_n, equal, etc.
#include <iterator>
#include <type_traits>
#include <utility>
#include <cstdint>



class FLOCBufferManager {
    public:
        void addPacket(const FlocPacket_t& packet);
        int checkqueueStatus();


    private:

        int retransmission_handler();
        int response_handler();
        int command_handler();
        int checkackID(uint8_t ackID);

        std::queue<FlocPacket_t> commandBuffer;
        std::queue<FlocPacket_t> responseBuffer;
        std::queue<FlocPacket_t> retransmissionBuffer;

        std::map<uint8_t, int> ackIDs;
        std::map<uint8_t, int> transmissionCounts;
        const int maxTransmissions = 5;
};



// #ifndef BUFFER_HPP
// #define BUFFER_HPP

// #include <stdint.h>
// #include "floc.hpp"

// // // Buffer structure for data
// // struct Buffer {
// //     uint8_t* data; // Pointer to the data array
// //     size_t size;   // Size of the buffer
// //     size_t index;  // Current index in the buffer
// // };

// void addPacket(const FlocPacket_t& packet);
// int checkqueueStatus();

// #endif