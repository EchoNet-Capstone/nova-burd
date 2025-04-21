#pragma once

#include <stdint.h>
#include "floc.hpp"


class FLOCBufferManager {
    public:
        void addPacket(const FlocPacket_t& packet);
        int checkqueueStatus();


    private:

        int retransmission_handler();
        int response_handler();
        int command_handler();

        std::queue<FlocPacket_t> commandBuffer;
        std::queue<FlocPacket_t> responseBuffer;
        std::queue<FlocPacket_t> retransmissionBuffer;
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