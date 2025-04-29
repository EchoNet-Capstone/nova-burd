#include <stdint.h>

#include <floc.hpp>
#include <nmv3_api.hpp>

#include "device_actions.hpp"
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

    while (MODEM_SERIAL_CONNECTION.available() > 0) {
        char modem_char = MODEM_SERIAL_CONNECTION.read();

        // Check for <CR><LF> sequence
        if (modem_char == '\n' && packetBuffer_modem_idx > 0 && packetBuffer_modem[packetBuffer_modem_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_modem[packetBuffer_modem_idx - 1] = 0;

            DeviceAction_t da;
            init_da(&da);

            packet_received_modem(packetBuffer_modem, packetBuffer_modem_idx - 1, &da);

            act_upon(&da);
            
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
    
}