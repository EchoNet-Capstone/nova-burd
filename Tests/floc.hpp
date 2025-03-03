#ifndef FLOC_H
#define FLOC_H

#include <cstdint>

// --- Configuration (Maximum Sizes) ---
#define FLOC_MAX_SIZE 64  // Maximum size of a complete FLOC packet
#define SERIAL_FLOC_MAX_SIZE 64 // Maximum size for a serial floc packet.

// --- Macros for field sizes (optional, for documentation) ---
#define FLOC_TTL_SIZE 4
#define FLOC_TYPE_SIZE 4
#define FLOC_NID_SIZE 16
#define FLOC_RES_SIZE 2
#define FLOC_PID_SIZE 6
#define FLOC_ADDR_SIZE 16

#define COMMAND_TYPE_SIZE 8

#define SERIAL_FLOC_TYPE_SIZE 8

// --- Calculate Maximum Data Sizes ---
#define FLOC_HEADER_COMMON_SIZE (sizeof(FlocHeader_t))
#define DATA_HEADER_SIZE        (sizeof(DataHeader_t))
#define COMMAND_HEADER_SIZE     (sizeof(CommandHeader_t))
#define RESPONSE_HEADER_SIZE    (sizeof(ResponseHeader_t))
#define ACK_HEADER_SIZE         (sizeof(AckHeader_t))

#define MAX_DATA_DATA_SIZE      (FLOC_MAX_SIZE - FLOC_HEADER_COMMON_SIZE - DATA_HEADER_SIZE)
#define MAX_COMMAND_DATA_SIZE   (FLOC_MAX_SIZE - FLOC_HEADER_COMMON_SIZE - COMMAND_HEADER_SIZE)
#define MAX_RESPONSE_DATA_SIZE  (FLOC_MAX_SIZE - FLOC_HEADER_COMMON_SIZE - RESPONSE_HEADER_SIZE)
#define MAX_ACK_DATA_SIZE       (FLOC_MAX_SIZE - FLOC_HEADER_COMMON_SIZE - ACK_HEADER_SIZE)

#pragma pack(push, 1)

// --- Define Small Header Structures ---
enum FlocPacketType_e : uint8_t{
    FLOC_DATA_TYPE = 0x0,
    FLOC_COMMAND_TYPE = 0x1,
    FLOC_ACK_TYPE = 0x2,
    FLOC_RESPONSE_TYPE = 0x3
};

enum CommandType_e: uint8_t {  // Example
    COMMAND_TYPE_1 = 0x1,
    COMMAND_TYPE_2 = 0x2,
    // ...
};

enum SerialFlocPacketType_e: uint8_t {
    SERIAL_BROADCAST_TYPE = 'B',
    SERIAL_UNICAST_TYPE   = 'U',
    // ...
};

struct FlocHeader_t {
    uint8_t ttl : FLOC_TTL_SIZE;
    FlocPacketType_e type : FLOC_TYPE_SIZE;
    uint16_t nid;  // Network ID
    uint8_t res : FLOC_RES_SIZE;
    uint8_t pid : FLOC_PID_SIZE;
    uint16_t dest_addr;
    uint16_t src_addr;
};

struct DataHeader_t {
    uint8_t size;
};

struct CommandHeader_t {
    CommandType_e command_type: COMMAND_TYPE_SIZE;
    uint8_t size;  // Size of the command data
};

struct AckHeader_t {
    uint8_t ack_pid;
};

struct ResponseHeader_t {
    uint8_t request_pid;
    uint8_t size;  // Size of the response data
};

// --- Define Packet Payload Structures ---
struct DataPacket_t {
    DataHeader_t header;
    uint8_t data[MAX_DATA_DATA_SIZE];
};

struct CommandPacket_t {
    CommandHeader_t header;
    uint8_t data[MAX_COMMAND_DATA_SIZE];
};

struct AckPacket_t {
    AckHeader_t header;
    // Optionally, add data if needed.
};

struct ResponsePacket_t {
    ResponseHeader_t header;
    uint8_t data[MAX_RESPONSE_DATA_SIZE];
};

// --- Define Unions (Complete Definitions) ---
union FlocPacketVariant_u {
    DataPacket_t     data;
    CommandPacket_t  command;
    AckPacket_t      ack;
    ResponsePacket_t response;
};

// --- Serial FLOC Structures ---
// Define the header first.
struct SerialFlocHeader_t {
    SerialFlocPacketType_e type : SERIAL_FLOC_TYPE_SIZE;
    uint8_t                size;
};

// Now, define the complete Serial FLOC packet payload types before using them in a union.
struct SerialUnicastPacket_t {
    uint16_t dest_addr;
    FlocPacketVariant_u floc_packet;
};

struct SerialBroadcastPacket_t {
    FlocPacketVariant_u floc_packet;
};

// Now define the union with complete types.
union SerialFlocPacketVariant_u {
    SerialBroadcastPacket_t broadcast;
    SerialUnicastPacket_t unicast;
};

// --- Now Define Complete Packet Structures ---
struct FlocPacket_t {
    FlocHeader_t header;
    FlocPacketVariant_u payload;
};

struct SerialFlocPacket_t {
    SerialFlocHeader_t header;
    SerialFlocPacketVariant_u payload;
};

#pragma pack(pop)

#endif
