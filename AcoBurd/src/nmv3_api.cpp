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

void print_packet(String packetBuffer, String packet_type) {
    Serial.println("Packet recieved.\n\tType : " + packet_type + "\n\tPacket data : " + packetBuffer);
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
    Serial.printf("$B%02u", bytes);
    for (int i = 0; i < bytes; i++) {
        Serial.printf(" %02X", (uint8_t)data[i]);
    }
    Serial.println();
    connection.printf("$B%02u", bytes);
    connection.write((uint8_t *)data, bytes);
}

void ping(HardwareSerial connection, uint8_t addr) {
    connection.printf("$P%03d", addr);
}

void parse_status_query_packet(QueryStatusResponseFullPacket_t* statusResponse) {
    char temp[6] = {0};
    memcpy(temp, statusResponse->addr, QUERY_STATUS_RESP_ADDR_MAX);
    temp[QUERY_STATUS_RESP_ADDR_MAX] = '\0';
    uint8_t node_addr = (uint8_t) atoi(temp);

    memcpy(temp, statusResponse->voltPayload, QUERY_STATUS_RESP_VOLT_PAYLOAD_MAX);
    temp[5] = '\0';
    long supply_voltage_meas = (uint8_t) atoi(temp);
    
    float supply_voltage = (float)(supply_voltage_meas) * 15.0f / 65536.0f;

    if (debug) {
        Serial.printf("Status query packet received.\r\n\tDevice addr : %03ld\r\n\tDevice Supply Voltage : %f\r\n", node_addr, supply_voltage);
    }

    floc_status_send(statusResponse);
}

void parse_set_address_packet(SetAddressResponsePacket_t* setAddressResponse) {
    char temp[4];
    memcpy(temp, setAddressResponse->addr, SET_ADDRESS_RESP_ADDR_MAX);
    temp[SET_ADDRESS_RESP_ADDR_MAX] = '\0';
    uint8_t new_addr = (uint8_t) atoi(temp);

    if (debug) {
        Serial.printf("Set address packet received.\r\n\tNew Device addr : %03ld\r\n", new_addr);
    }

    set_modem_id(new_addr);
    display_modem_id(get_modem_id());
}

void parse_unicast_packet(uint8_t* packetBuffer, uint8_t size) {
    // TODO : implement?
}

void parse_broadcast_packet(BroadcastMessageResponsePacket_t* broadcast) {
    Serial.printf("Parsing broadcast packet...\r\n");

    char temp[4];
    uint8_t src_addr;
    uint8_t bytes;

    memcpy(temp, broadcast->header.addr, BROADCAST_RESP_ADDR_MAX);
    temp[BROADCAST_RESP_ADDR_MAX] = '\0';
    // Serial.printf("temp: %s\r\n", temp);
    src_addr = (uint8_t) atoi(temp);

    memset(temp, 0, 4);
    memcpy(temp, broadcast->header.dataSize, 2);
    temp[BROADCAST_RESP_DATA_SIZE_MAX] = '\0';
    // Serial.printf("temp: %s\r\n", temp);
    bytes = (uint8_t) atoi(temp);

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

void parse_unicast_packet(String packetBuffer) {
    long bytes = packetBuffer.substring(UNICAST_BYTE_LENGTH_START, UNICAST_BYTE_LENGTH_END).toInt();

    String packetData = packetBuffer.substring(UNICAST_PACKET_DATA_START, UNICAST_PACKET_DATA_START + bytes);
    if (debug) {
        Serial.printf("Unicast packet received.\r\n\tPacket Bytes : %ld\r\n\tPacket data : ", bytes);
        Serial.print(packetData + "\r\n");
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
    if (debug) {
        Serial.printf("PKT RECV (%02u bytes): [", size);
        for(int i = 0; i < size; i++){
            Serial.printf("%02x, ", packetBuffer[i]);
        }
        Serial.printf("]\r\n");
    }

    if (size < 1) {
        // Should never happen over serial connection.
        return;
    }

    ModemPacket_t* pkt = (ModemPacket_t*) packetBuffer;

    if (pkt->type == 'E' && size == 1) {
        if (debug) Serial.println("Error packet received.");
        return;
    }

    if (size < 2) {
        if (debug) Serial.println("Packet invalid, size too small.");
        return;
    }

    if (pkt->type == '$') {
        ModemLocalResponsePacket_t* localResp = (ModemLocalResponsePacket_t* )&pkt->payload;

        Serial.printf("Local Echo...");
        // Local Echo
        switch (localResp->type) {
            case '?':
                break;
            case 'A':
                break;
            case 'B':
                if (size == BROADCAST_CMD_LOCAL_RESP_MAX) {
                    // if (debug) {
                    //     Serial.printf("Broadcast of %02ld bytes sent.\r\n",
                    //     packetBuffer.substring(BROADCAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                    //                            BROADCAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt());
                    // }
                }
                break;
            case 'M':
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
            case 'P':
                if (size == PING_CMD_LOACL_RESP_MAX) {
                    // if (debug) {
                    //     Serial.printf("Ping to modem %03ld sent.\r\n", 
                    //     packetBuffer.substring(PING_LOCAL_ECHO_DEST_ADDR_START, 
                    //                            PING_LOCAL_ECHO_DEST_ADDR_END).toInt());
                    // }
                }
                break;
            case 'U':
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
            case 'V':
                break;

            default:
                break;
                // if (debug) {
                //     Serial.printf("Unhandled packet type [modem].\r\n\tPrefix : %c\r\n", packetBuffer.charAt(1));
                //     Serial.println("\tFull packet : " + packetBuffer);
                // }
        }
    } else if (pkt->type) {
        Serial.printf("Response packet...\r\n");

        ModemResponsePacket_t* response = (ModemResponsePacket_t*)&pkt->payload;

        switch (response->type) {
            case '?':
                break;
            case 'A':{
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
            case 'B': {
                BroadcastMessageResponsePacket_t* broadcast = (BroadcastMessageResponsePacket_t*) &response->response;
                parse_broadcast_packet(broadcast);
                break;
            }
            case 'M':
                // TODO : Handle unicast with ack? Does this exist with # prefix?
                break;
            case 'P':
                
                break;
            case 'R': {
                RangeDataResponsePacket_t* rangeResponse = (RangeDataResponsePacket_t*) &response->response;

                if (size == RANGE_RESP_MAX) {
                    parse_ping_packet(rangeResponse);
                }
                break;
            }
            case 'T':
                if (size == TIMEOUT_PACKET_LENGTH) {
                    if (debug) {
                        Serial.println("Timeout.");
                    }
                }
                break;
            case 'U':
                    parse_unicast_packet(packetBuffer, size);
                break;
            case 'V':
                // TODO : Handling will require modifying Broadcast packet recv handling
                break;

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

    uint8_t pkt_type = *(packetBuffer++);

    SerialFlocPacket_t* pkt = (SerialFlocPacket_t*)(packetBuffer);

    if (pkt_type == '$') {

        switch (pkt->header.type) {
            // Broadcast the data received on the serial line
            case 'B': {
                SerialBroadcastPacket_t* broadcastPacket = (SerialBroadcastPacket_t* )&pkt->payload;
                broadcast(MODEM_SERIAL_CONNECTION, (char*) broadcastPacket, pkt->header.size);
                // display_modem_packet_data(packetBuffer);
                break;
            }
            case 'U':
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