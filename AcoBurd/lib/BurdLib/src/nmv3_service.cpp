#include <stdint.h>

#include <floc.hpp>
#include <nmv3_api.hpp>

#include "bloomfilter.hpp"
#include "device_actions.hpp"
#include "display.hpp"
#include "globals.hpp"
#include "neighbor.hpp"
#include "services.hpp"

#include "nmv3_service.hpp"

extern Service modemServiceDesc;

// Packet buffer for data received from the acoustic modem serial line
static uint8_t packetBuffer_modem[sizeof(ModemPacket_t) + 2] = {0};
static uint8_t packetBuffer_modem_idx = 0;

HardwareSerial& modem_connection = MODEM_SERIAL_CONNECTION;

void
modemService(
    void
){
    static bool in_frame = false;

    modemServiceDesc.busy = false;

    ParseResult r;

    while (modem_connection.available() > 0) {
        char modem_char = modem_connection.read();

        // Check for <CR><LF> sequence
        if (modem_char == '\n' && packetBuffer_modem_idx > 0 && packetBuffer_modem[packetBuffer_modem_idx - 1] == '\r') {
            // Remove the <CR> from the buffer
            packetBuffer_modem[packetBuffer_modem_idx - 1] = 0;

            uint8_t packet_size = packetBuffer_modem_idx - 1;

            if (bloom_check_packet(packetBuffer_modem, packet_size)) {
            #ifdef DEBUG_ON
                Serial.printf("Duplicate packet (raw hash), dropping.\n");
            #endif
            }
                
            maybe_reset_bloom_filter();
            bloom_add_packet(packetBuffer_modem, packet_size);

            r = packet_received_modem(packetBuffer_modem, packet_size);
            
            // Packet Received Parse the response;
            switch (r.type) {
                case BROAD_RECV_TYPE:
                    init_da();
        
                    floc_broadcast_received(r.broadcast.payload, r.broadcast.payload_size);
        
                    act_upon();
                    break;
                case PING_RESP_TYPE:
                    // TODO: Handle ping response

                    neighborManager.update_neighbors(r.ping.src_addr, r.ping.meter_range);
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
    
            memset(packetBuffer_modem, 0 , sizeof(packetBuffer_modem)); // Clear the buffer
            packetBuffer_modem_idx = 0;

            in_frame = false;
        } else {
            if (packetBuffer_modem_idx >= sizeof(packetBuffer_modem) - 1) {
                // Some error has occurred, clear the packet
                memset(packetBuffer_modem, 0 , sizeof(packetBuffer_modem));
                packetBuffer_modem_idx = 0;

                in_frame = false;
                continue;
            }

            // Append character to the buffer

            if (!in_frame && (modem_char == '$' || modem_char == '#')){
                in_frame = true;
            } 
            
            if (in_frame){
                packetBuffer_modem[packetBuffer_modem_idx] = modem_char;
                packetBuffer_modem_idx++;
            }
        }

        modemServiceDesc.busy = true;
    }
}

void
nmv3_init(
    void
){
#ifdef DEBUG_ON // DEBUG_ON
    Serial.printf("Initializing NMV3...\r\n");
#endif // DEBUG_ON

    // Serial connection to modem
    modem_connection.begin(9600, SERIAL_8N1);

    delay(100);

    // hash
    uint8_t new_modem_id = (get_device_id() * 31 + get_network_id()) & 0xFF;

    set_address(new_modem_id);
}