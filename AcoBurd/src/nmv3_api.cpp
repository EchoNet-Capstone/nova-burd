#include "nmv3_api.hpp"
#include "floc.hpp"

GET_SET_FUNC_DEF(uint8_t, modem_id, 0)

// Supported Modem Commands (Link Quality Indicator OFF)
//  Query Status                                DONE
//  Set Address                                 DONE
//  Broadcast Message                           DONE
//  Unicast Message with Ack                    DONE
//  Ping                                        DONE
//  Unicast Message                             DONE
//  Battery Voltage and Noise Measurement*      TBD
//  Error
//  Timeout

int fieldToInt(char* field, uint8_t field_len){
    char temp[field_len + 1] = {0};

    memcpy(temp, field, field_len);
    temp[field_len] = '\0';

    return atoi(temp);
}

// BEGIN MODEM SERIAL CONNECTION FUNCTIONS -----------------

void query_status(HardwareSerial connection) {
    connection.print("$?");
}

void set_address(HardwareSerial connection, uint8_t addr) {
    connection.printf("$A%03d", addr);
}

uint8_t get_modem_address() {
    return modem_id;
}

void broadcast(HardwareSerial connection, char *data, uint8_t bytes) {
    /*Serial.printf("$B%02u", bytes);
    for (int i = 0; i < bytes; i++) {
        Serial.printf(" %02X", (uint8_t)data[i]);
    }
    Serial.println();*/
    connection.printf("$B%02u", bytes);
    connection.write((uint8_t *)data, bytes);
}

void ping(HardwareSerial connection, uint8_t addr) {
    connection.printf("$P%03d", addr);
}

void parse_status_query_packet(QueryStatusResponseFullPacket_t* statusResponse) {
    uint8_t node_addr = (uint8_t) fieldToInt((char*) statusResponse->addr, QUERY_STATUS_RESP_ADDR_MAX);

    uint16_t supply_voltage_meas = (uint16_t) fieldToInt((char*) statusResponse->voltPayload, QUERY_STATUS_RESP_VOLT_PAYLOAD_MAX);
    
    float supply_voltage = (float)(supply_voltage_meas) * 15.0f / 65536.0f;

    if (debug) {
        Serial.printf("Status query packet received.\r\n\tDevice addr : %03ld\r\n\tDevice Supply Voltage : %f\r\n", node_addr, supply_voltage);
    }

    floc_status_send(statusResponse);
}

void parse_set_address_packet(SetAddressResponsePacket_t* setAddressResponse) {
    uint8_t new_addr = (uint8_t) fieldToInt((char*) setAddressResponse->addr, SET_ADDRESS_RESP_ADDR_MAX);

    if (debug) {
        Serial.printf("Set address packet received.\r\n\tNew Device addr : %03ld\r\n", new_addr);
    }

    set_modem_id(new_addr);
    display_modem_id(get_modem_id());
}

void parse_broadcast_packet(BroadcastMessageResponsePacket_t* broadcast) {
    Serial.printf("Parsing broadcast packet...\r\n");

    uint8_t src_addr = fieldToInt((char*) broadcast->header.addr, BROADCAST_RESP_ADDR_MAX);
    uint8_t bytes = fieldToInt((char*) broadcast->header.dataSize, BROADCAST_RESP_DATA_SIZE_MAX);

    Serial.printf("Src_addr: %d, Size: %d\r\n", src_addr, bytes);

    uint8_t *message = (uint8_t*) &broadcast->message;

    // if (debug) {
    //     Serial.printf("Broadcast packet received.\r\n\tPacket src addr : %03ld\r\n\tPacket Bytes : %ld\r\n\tPacket data : ", src_addr, bytes);
    //     for(int i = 0; i < bytes; i++){
    //         Serial.printf("%X", broadcastBuffer[i]);
    //     }
    //     Serial.printf("\r\n");
    // }

    floc_broadcast_received(message, bytes);
}

void parse_unicast_packet(UnicastResponsePacket_t* unicast) {
    uint8_t bytes = (uint8_t) fieldToInt((char*) unicast->dataSize, UNICAST_RESP_DATA_SIZE_MAX);

    char message[bytes+1] = {0};
    memcpy(message, unicast->payload, bytes);

    if (debug) {
        Serial.printf("Unicast packet received.\r\n\tPacket Bytes : %ld\r\n\tPacket data : ", bytes);
        Serial.printf("%s\r\n", message);
    }
}

// Also handles ack packets from Unicast with ack command
void parse_ping_packet(RangeDataResponsePacket_t* rangeResponse) {
    uint8_t src_addr = (uint8_t) fieldToInt((char*) rangeResponse->addr, RANGE_RESP_ADDR_MAX);
    uint16_t ping_propogation_counter = (uint16_t) fieldToInt((char*) rangeResponse->rangePayload, RANGE_RESP_PAYLOAD_MAX);

    float meter_range = static_cast<float>(ping_propogation_counter) * SOUND_SPEED * 3.125e-5;

    if (debug) {
        Serial.printf("Ping (or ACK) packet received.\r\n\tAddr : %03ld\r\n\tRange (m) : %f\r\n", src_addr, meter_range);
    }
}

void packet_received_modem(uint8_t* packetBuffer, uint8_t size) {
    /*if (debug) {
        Serial.printf("PKT RECV (%02u bytes): [", size);
        for(int i = 0; i < size; i++){
            // Serial.printf("%02x, ", packetBuffer[i]);
            Serial.printf("%c, ", packetBuffer[i]);
        }
        Serial.printf("]\r\n");
    }*/
    
    if (size < 1) {
        // Should never happen over serial connection.
        return;
    }

    if (packetBuffer[0] == ERROR_TYPE && size == ERROR_MAX) { // 'E'
        if (debug) Serial.println("Error packet received.");
        return;
    }

    if (size < MODEM_MSG_MIN) {
        if (debug) Serial.println("Packet invalid, size too small.");
        return;
    }

    ModemPacket_t* pkt = (ModemPacket_t*) packetBuffer;

    if (pkt->type == MODEM_LOCAL_RESPONSE_TYPE) { // '$'
        ModemLocalResponsePacket_t* localResp = (ModemLocalResponsePacket_t* )&pkt->payload;

        Serial.printf("Local Echo...");

        // Local Echo
        switch (localResp->type) {
            case BROADCAST_CMD_LOCAL_RESP_TYPE: // 'B'
                if (size == BROADCAST_CMD_LOCAL_RESP_MAX) {
                    // if (debug) {
                    //     Serial.printf("Broadcast of %02ld bytes sent.\r\n",
                    //     packetBuffer.substring(BROADCAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                    //                            BROADCAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt());
                    // }
                }
                break;
            case CHN_IMP_CMD_LOCAL_RESP_TYPE: // 'C' Will also be reached if it's a Corrected Error Local Response Packet
                break;
            case ECHO_MSG_CMD_LOCAL_RESP_TYPE: // 'E'
                break;
            case UNICAST_ACK_CMD_LOCAL_RESP_TYPE: // 'M'
                if (size == UNICAST_ACK_CMD_LOCAL_RESP_MAX) {
                    // if (debug) {
                    //     Serial.printf("Unicast (with ACK) of %02ld bytes to address %03ld sent.\r\n",
                    //     packetBuffer.substring(UNICAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                    //                            UNICAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt(),
                    //     packetBuffer.substring(UNICAST_LOCAL_ECHO_DEST_ADDR_START, 
                    //                            UNICAST_LOCAL_ECHO_DEST_ADDR_END).toInt());
                    // }
                }
                break;
            case PING_CMD_LOCAL_RESP_TYPE: // 'P'
                if (size == PING_CMD_LOCAL_RESP_MAX) {
                    if (debug) {
                        Serial.print("Ping to modem "); 
                        for (int i = 0; i < PING_CMD_LOCAL_RESP_ADDR_MAX; i++) {
                            Serial.printf("%c", ((PingLocalResponsePacket_t*) &localResp->response)->addr[i]);
                        }
                        Serial.print("sent.\r\n"); 
                    }
                }
                break;
            case RESET_CMD_LOCAL_RESP_TYPE: // 'R'
                break;
            case TEST_MSG_CMD_LOCAL_RESP_MAX: // 'T'
                break;
            case UNICAST_CMD_LOCAL_RESP_TYPE: // 'U'
                if (size == UNICAST_CMD_LOCAL_RESP_MAX) {
                    // if (debug) {
                    //     Serial.printf("Unicast of %02ld bytes to address %03ld sent.\r\n",
                    //     packetBuffer.substring(UNICAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                    //                            UNICAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt(),
                    //     packetBuffer.substring(UNICAST_LOCAL_ECHO_DEST_ADDR_START, 
                    //                            UNICAST_LOCAL_ECHO_DEST_ADDR_END).toInt());
                    // }
                }
                break;
            case VOLT_NOISE_MSR_CMD_LOCAL_RESP_TYPE: // 'V'
                break;

            default:
                break;
                // if (debug) {
                //     Serial.printf("Unhandled packet type [modem].\r\n\tPrefix : %c\r\n", packetBuffer.charAt(1));
                //     Serial.println("\tFull packet : " + packetBuffer);
                // }
        }
    } else if (pkt->type == MODEM_RESPONSE_TYPE) { // '#'
        Serial.printf("Response packet...\r\n");

        ModemResponsePacket_t* response = (ModemResponsePacket_t*)&pkt->payload;

        switch (response->type) {
            case QUERY_STATUS_RESP_TYPE: // 'A' Will also be reached if it's a Set Address Response Packet
            { 
                QueryStatusResponsePacket_t* statusResponse = (QueryStatusResponsePacket_t*) &response->response;
                if (size == QUERY_STATUS_RESP_MAX) {
                    QueryStatusResponseFullPacket_t* fullStatus = (QueryStatusResponseFullPacket_t*) &statusResponse->status;
                    parse_status_query_packet(fullStatus);
                } else if (size == SET_ADDRESS_RESP_MAX) {
                    SetAddressResponsePacket_t* setAddressResponse = (SetAddressResponsePacket_t*) &statusResponse->status;
                    parse_set_address_packet(setAddressResponse);
                }
                break;
            }
            case BROADCAST_RESP_TYPE: // 'B' 
            {  
                BroadcastMessageResponsePacket_t* broadcast = (BroadcastMessageResponsePacket_t*) &response->response;
                parse_broadcast_packet(broadcast);
                break;
            }
            case RANGE_RESP_TYPE: // 'R'
            {
                RangeDataResponsePacket_t* rangeResponse = (RangeDataResponsePacket_t*) &response->response;

                if (size == RANGE_RESP_MAX) {
                    parse_ping_packet(rangeResponse);
                }
                break;
            }
            case TIMEOUT_RESP_TYPE: // 'T' 
                if (size == TIMEOUT_MAX) {
                    if (debug) {
                        Serial.println("Timeout.");
                    }
                }
                break;
            case UNICAST_RESP_TYPE: // 'U' 
            {
                UnicastResponsePacket_t* unicast = (UnicastResponsePacket_t*) &response->response;
                parse_unicast_packet(unicast);
                break;
            }

            default:
                break;
                // if (debug) {
                //     Serial.printf("Unhandled packet type [modem].\r\n\tPrefix : %c\r\n", packetBuffer.charAt(1));
                //     Serial.println("\tFull packet : " + packetBuffer);
                // }
        }
    } else {
        Serial.printf("Error...\r\n");
        // Packet does not follow modem response structure (starts with $ or #)
        // if (debug) print_packet(packetBuffer, "Unknown prefix [modem packet]");
        return;
    }
}

// BEGIN NeST SERIAL CONNECTION FUNCTIONS -----------------

void packet_received_nest(uint8_t* packetBuffer, uint8_t size) {
    
    if (size < 3) {
        // Need a prefix character, a casting type, and at least one byte of data e.g. $BX for a broadcast with data 'X'
        if (debug) Serial.println("NeST packet too small. Minimum size : 3.");
        return;
    }

    uint8_t pkt_type = *(packetBuffer++); // Remove '$' prefix

    SerialFlocPacket_t* pkt = (SerialFlocPacket_t*)(packetBuffer);

    if (pkt_type == '$') {
        switch (pkt->header.type) {
            // Broadcast the data received on the serial line
            case SERIAL_BROADCAST_TYPE: // 'B'
            {
                SerialBroadcastPacket_t* broadcastPacket = (SerialBroadcastPacket_t* )&pkt->payload;
                broadcast(MODEM_SERIAL_CONNECTION, (char*) broadcastPacket, pkt->header.size);
                // display_modem_packet_data(packetBuffer);
                break;
            }
            case SERIAL_UNICAST_TYPE:   // 'U'
                // TODO : need to extract dst from packet in order to send packet
                // May not need to implement, depending on networking strategy
                break;
            default:
                if (debug) {
                    Serial.printf("Unhandled packet type [NeST] : prefix [%c]\r\n", packetBuffer[1]);
                    Serial.printf("Full packet : %s\r\n", packetBuffer);
                }
        }
    } else {
        // Packet does not follow nest prefix structure (starts with $)
        // if (debug) print_packet(packetBuffer, "Unknown prefix [NeST packet]");
        return;
    }
}