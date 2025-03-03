#include "heltec_serial_api.hpp"

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

uint8_t modem_id = 0;

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

void parse_status_query_packet(uint8_t* packetBuffer, uint8_t size) {
    char temp[6];
    memcpy(temp, packetBuffer + STATUS_QUERY_NODE_ADDR_START, STATUS_QUERY_NODE_ADDR_END - STATUS_QUERY_NODE_ADDR_START);
    temp[4] = '\0';
    long node_addr = (uint8_t) atoi(temp);

    memcpy(temp, packetBuffer + STATUS_QUERY_NODE_ADDR_START, STATUS_QUERY_NODE_ADDR_END - STATUS_QUERY_NODE_ADDR_START);
    temp[5] = '\0';
    long supply_voltage_meas = (uint8_t) atoi(temp);
    
    float supply_voltage = static_cast<float>(supply_voltage_meas) * 15.0f / 65536.0f;

    if (debug) {
        Serial.printf("Status query packet received.\r\n\tDevice addr : %03ld\r\n\tDevice Supply Voltage : %f\r\n", node_addr, supply_voltage);
    }

    floc_status_send(packetBuffer, size);
}

void parse_set_address_packet(uint8_t *packetBuffer, uint8_t size) {
    char temp[4];
    memcpy(temp, packetBuffer + SET_ADDRESS_ADDR_START, SET_ADDRESS_ADDR_END - SET_ADDRESS_ADDR_START);
    temp[3] = '\0';
    long new_addr = (uint8_t) atoi(temp);

    if (debug) {
        Serial.printf("Set address packet received.\r\n\tNew Device addr : %03ld\r\n", new_addr);
    }

    modem_id = new_addr;
    display_modem_id(modem_id);
}

void parse_unicast_packet(uint8_t* packetBuffer, uint8_t size) {
    // TODO : implement?
}

void parse_broadcast_packet(uint8_t* packetBuffer, uint8_t size) {
    Serial.printf("Parsing broadcast packet...\r\n");
    char temp[4];
    uint8_t src_addr;
    uint8_t bytes;
    memcpy(temp, packetBuffer + BROADCAST_SRC_ADDR_START, 3);
    temp[4] = '\0';
    // Serial.printf("temp: %s\r\n", temp);
    src_addr = (uint8_t) atoi(temp);

    memset(temp, 0, 4);
    memcpy(temp, packetBuffer + BROADCAST_BYTE_LENGTH_START, 2);
    temp[3] = '\0';
    // Serial.printf("temp: %s\r\n", temp);
    bytes = (uint8_t) atoi(temp);

    Serial.printf("Src_addr: %d, Size: %d\r\n", src_addr, bytes);

    uint8_t *broadcastBuffer = packetBuffer + BROADCAST_PACKET_DATA_START;

    // if (debug) {
    //     Serial.printf("Broadcast packet received.\r\n\tPacket src addr : %03ld\r\n\tPacket Bytes : %ld\r\n\tPacket data : ", src_addr, bytes);
    //     for(int i = 0; i < bytes; i++){
    //         Serial.printf("%X", broadcastBuffer[i]);
    //     }
    //     Serial.printf("\r\n");
    // }

    uint8_t* packetData = (uint8_t*) malloc(bytes);
    memcpy(packetData, broadcastBuffer, bytes);
    floc_broadcast_received(packetData, bytes);
    free(packetData);
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
void parse_ping_packet(uint8_t *packetBuffer, uint8_t size) {
    String packetParser = "";
    
    for (int i = PING_ADDR_START; i < PING_ADDR_END; i++) {
        packetParser += (char)packetBuffer[i];
    }
    long ping_addr = packetParser.toInt();
    packetParser = "";

    for (int i = PING_PROPOGATION_COUNTER_START; i < PING_PROPOGATION_COUNTER_END; i++) {
        packetParser += (char)packetBuffer[i];
    }
    long ping_propogation_counter = packetParser.toInt();
    packetParser = "";

    float meter_range = static_cast<float>(ping_propogation_counter) * SOUND_SPEED * 3.125e-5;

    if (debug) {
        Serial.printf("Ping (or ACK) packet received.\r\n\tAddr : %03ld\r\n\tRange (m) : %f\r\n", ping_addr, meter_range);
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

    if (packetBuffer[0] == 'E' && size == 1) {
        if (debug) Serial.println("Error packet received.");
        return;
    }


    if (size < 2) {
        if (debug) Serial.println("Packet invalid, size too small.");
        return;
    }

    if (packetBuffer[0] == '$') {
        Serial.printf("Local Echo...");
        // Local Echo
        switch (packetBuffer[1]) {
            case '?':
                break;
            case 'A':
                break;
            case 'B':
                if (size == BROADCAST_LOCAL_ECHO_LENGTH) {
                    // if (debug) {
                    //     Serial.printf("Broadcast of %02ld bytes sent.\r\n",
                    //     packetBuffer.substring(BROADCAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                    //                            BROADCAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt());
                    // }
                }
                break;
            case 'M':
                if (size == UNICAST_LOCAL_ECHO_LENGTH) {
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
                if (size == PING_LOCAL_ECHO_LENGTH) {
                    // if (debug) {
                    //     Serial.printf("Ping to modem %03ld sent.\r\n", 
                    //     packetBuffer.substring(PING_LOCAL_ECHO_DEST_ADDR_START, 
                    //                            PING_LOCAL_ECHO_DEST_ADDR_END).toInt());
                    // }
                }
                break;
            case 'U':
                if (size == UNICAST_LOCAL_ECHO_LENGTH) {
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
    } else if (packetBuffer[0] == '#') {
        Serial.printf("Response packet...\r\n");
        switch (packetBuffer[1]) {
            case '?':
                break;
            case 'A':
                if (size == STATUS_QUERY_PACKET_LENGTH) {
                    parse_status_query_packet(packetBuffer, size);
                } else if (size == SET_ADDRESS_PACKET_LENGTH) {
                    parse_set_address_packet(packetBuffer, size);
                }
                break;
            case 'B':
                    parse_broadcast_packet(packetBuffer, size);
                break;
            case 'M':
                // TODO : Handle unicast with ack? Does this exist with # prefix?
                break;
            case 'P':
                
                break;
            case 'R':
                if (size == PING_PACKET_LENGTH) {
                    parse_ping_packet(packetBuffer, size);
                }
                break;
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

    if (packetBuffer[0] == '$') {
        char *transmission_data_start = ((char *)packetBuffer + 2);

        switch (packetBuffer[1]) {
            // Broadcast the data received on the serial line
            case 'B':
                broadcast(MODEM_SERIAL_CONNECTION, transmission_data_start, size - 2);
                // display_modem_packet_data(packetBuffer);
                break;
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