#ifndef GLOBALS_H
#define GLOBALS_H

#define debug true
#define ON_DEVICE

#define NEST_SERIAL_CONNECTION Serial
#define MODEM_SERIAL_CONNECTION Serial1

// Speed of sound in m/s
// #define SOUND_SPEED 343     // Sound speed through air
#define SOUND_SPEED 1500    // Sound speed through water

// Acoustic Modem Serial Transmission Byte Lengths
#define STATUS_QUERY_PACKET_LENGTH 43
#define SET_ADDRESS_PACKET_LENGTH 5
#define PING_PACKET_LENGTH 11 // Also length for ack response from Unicast w/ ack
#define BATTERY_VOLT_NOISE_MEAS_PACKET_LENGTH 0 // Test and update value
#define TIMEOUT_PACKET_LENGTH 3

#define PACKET_BUFFER_SIZE 128
#define TTL_START 3

// Starts inclusive, ends exclusive

//Status Query
#define STATUS_QUERY_NODE_ADDR_START 2
#define STATUS_QUERY_NODE_ADDR_END 5
#define STATUS_QUERY_SUPPLY_VOLTAGE_START 6
#define STATUS_QUERY_SUPPLY_VOLTAGE_END 11
#define STATUS_QUERY_RELEASE_START 11
#define STATUS_QUERY_RELEASE_END 23
#define STATUS_QUERY_BUILD_DATE_START 23
#define STATUS_QUERY_BUILD_DATE_END 43

// Set Address
#define SET_ADDRESS_ADDR_START 2
#define SET_ADDRESS_ADDR_END 5

// Broadcast
#define BROADCAST_SRC_ADDR_START 2
#define BROADCAST_SRC_ADDR_END 5
#define BROADCAST_BYTE_LENGTH_START 5
#define BROADCAST_BYTE_LENGTH_END 7
#define BROADCAST_PACKET_DATA_START 7

#define BROADCAST_LOCAL_ECHO_LENGTH 4
#define BROADCAST_LOCAL_ECHO_BYTE_LENGTH_START 2
#define BROADCAST_LOCAL_ECHO_BYTE_LENGTH_END 4

// Unicast
#define UNICAST_BYTE_LENGTH_START 2
#define UNICAST_BYTE_LENGTH_END 4
#define UNICAST_PACKET_DATA_START 4

#define UNICAST_LOCAL_ECHO_LENGTH 7
#define UNICAST_LOCAL_ECHO_DEST_ADDR_START 2
#define UNICAST_LOCAL_ECHO_DEST_ADDR_END 5
#define UNICAST_LOCAL_ECHO_BYTE_LENGTH_START 5
#define UNICAST_LOCAL_ECHO_BYTE_LENGTH_END 7

// Ping
#define PING_ADDR_START 2
#define PING_ADDR_END 5
#define PING_PROPOGATION_COUNTER_START 6
#define PING_PROPOGATION_COUNTER_END 11

#define PING_LOCAL_ECHO_LENGTH 5
#define PING_LOCAL_ECHO_DEST_ADDR_START 2
#define PING_LOCAL_ECHO_DEST_ADDR_END 5

static inline uint16_t htons(uint16_t val) {
    return __builtin_bswap16(val);
}

static inline uint16_t ntohs(uint16_t val) {
    return __builtin_bswap16(val);
}

#endif