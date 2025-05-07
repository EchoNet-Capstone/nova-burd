#include <EEPROM.h>

#include <stdint.h>

#include <floc.hpp>
#include <nmv3_api.hpp>

#include "device_actions.hpp"
#include "display.hpp"
#include "globals.hpp"
#include "services.hpp"

extern Service modemServiceDesc;

// Packet buffer for data received from the acoustic modem serial line
static uint8_t packetBuffer_modem[FLOC_MAX_SIZE] = {0};
static uint8_t packetBuffer_modem_idx = 0;

void
modemService(
    void
){
    modemServiceDesc.busy = false;

    ParseResult r;

    while (MODEM_SERIAL_CONNECTION.available() > 0) {
        char modem_char = MODEM_SERIAL_CONNECTION.read();

        // Check for <CR><LF> sequence
        if (modem_char == '\n' && packetBuffer_modem_idx > 0 && packetBuffer_modem[packetBuffer_modem_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_modem[packetBuffer_modem_idx - 1] = 0;

            r = packet_received_modem(packetBuffer_modem, packetBuffer_modem_idx - 1);

            // TODO : Handle the result of the packet processing
            
            memset(packetBuffer_modem, 0 , sizeof(packetBuffer_modem)); // Clear the buffer
            packetBuffer_modem_idx = 0;
        } else {
            if (packetBuffer_modem_idx >= sizeof(packetBuffer_modem)) {
                // Some error has occurred, clear the packet
                memset(packetBuffer_modem, 0 , sizeof(packetBuffer_modem));
                packetBuffer_modem_idx = 0;
            }
            // Append character to the buffer
            packetBuffer_modem[packetBuffer_modem_idx] = modem_char;
            packetBuffer_modem_idx++;
        }

        modemServiceDesc.busy = true;
    }

    switch (r.type) {
        case BROAD_RECV_TYPE:
            DeviceAction_t da;
            init_da(&da);

            floc_broadcast_received(r.broadcast.payload, r.broadcast.payload_size, &da);

            act_upon(&da);
            break;
        case PING_RESP_TYPE:
            // TODO: Handle ping response
            break;
        case STATUS_QUERY_TYPE:
            // TODO: handle status query response
            break;
        case SET_ADDR_TYPE:
            // TODO: handle set_addr_type
            break;
        default:
            break;
    }
}

#ifdef RECV_SERIAL_NEST // RECV_SERIAL_NEST
extern Service nestSerialServiceDesc;

// Packet buffer for data received from the ship terminal (NeST) serial line
static uint8_t packetBuffer_nest[SERIAL_FLOC_MAX_SIZE] = {0};
static uint8_t packetBuffer_nest_idx = 0;

void
nestSerialService(
    void
){
    nestSerialServiceDesc.busy = false;

    while (NEST_SERIAL_CONNECTION.available() > 0) {
        char nest_char = NEST_SERIAL_CONNECTION.read();

        // Check for <CR><LF> sequence
        if (nest_char == '\n' && packetBuffer_nest_idx > 0 && packetBuffer_nest[packetBuffer_nest_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_nest[packetBuffer_nest_idx - 1] = 0;

            DeviceAction_t da;
            init_da(&da);

            packet_received_nest(packetBuffer_nest, packetBuffer_nest_idx - 1, &da);

            act_upon(&da);
            
            memset(packetBuffer_nest, 0 , sizeof(packetBuffer_nest)); // Clear the buffer
            packetBuffer_nest_idx = 0;
        } else {
            if (packetBuffer_nest_idx >= sizeof(packetBuffer_nest)) {
                // Some error has occurred, clear the packet
                memset(packetBuffer_nest, 0 , sizeof(packetBuffer_nest));
                packetBuffer_nest_idx = 0;
            }
            // Append character to the buffer
            packetBuffer_nest[packetBuffer_nest_idx] = nest_char;
            packetBuffer_nest_idx++;
        }

        nestSerialServiceDesc.busy = true;
    }
}
#endif

void
nmv3_init(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Initializing NMV3...\r\n");
#endif // DEBUG_ON

    uint16_t t_device_id;
    uint16_t t_network_id;

    EEPROM.get(DEVICE_ID_ADDR, t_device_id);
    EEPROM.get(NETWORK_ID_ADDR, t_network_id);

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Got DID, NID. Setting Modem ID...\r\n");
#endif // DEBUG_ON

    // hash
    uint8_t new_modem_id = (t_device_id * 31 + t_network_id) & 0xFF;

    set_address(MODEM_SERIAL_CONNECTION, new_modem_id);

#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Modem ID set...\r\n");
#endif // DEBUG_ON
}