#include "modem_api.hpp"

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

void query_status(HardwareSerial connection) {
    connection.print("$?");
}

void set_address(HardwareSerial connection, int8_t addr) {
    connection.printf("$A%03d", addr);
}

uint8_t get_modem_address() {
    return modem_id;
}

void broadcast(HardwareSerial connection, char *data, int8_t bytes) {
    connection.printf("$B%02d", bytes);
    connection.print(data);
}

void ping(HardwareSerial connection, int8_t addr) {
    connection.printf("$P%03d", addr);
}

void parse_status_query_packet(String packetBuffer) {
    long node_addr = packetBuffer.substring(STATUS_QUERY_NODE_ADDR_START, STATUS_QUERY_NODE_ADDR_END).toInt();
    long supply_voltage_meas = packetBuffer.substring(STATUS_QUERY_SUPPLY_VOLTAGE_START, STATUS_QUERY_SUPPLY_VOLTAGE_END).toInt(); // TODO : This is returning 0, debug?
    float supply_voltage = static_cast<float>(supply_voltage_meas) * 15.0f / 65536.0f;
    String release_version = packetBuffer.substring(STATUS_QUERY_RELEASE_START, STATUS_QUERY_RELEASE_END);
    String build_date_time = packetBuffer.substring(STATUS_QUERY_BUILD_DATE_START, STATUS_QUERY_BUILD_DATE_END);

    if (debug) {
        Serial.printf("Status query packet received.\r\n\tDevice addr : %03ld\r\n\tDevice Supply Voltage : %f\r\n\tDevice Release Version : ", node_addr, supply_voltage);
        Serial.print(release_version + "\r\n\tDevice Build Date + Time [BYYYY-MM-DDThh:mm:ss] : " + build_date_time + "\r\n");
    }
}

void parse_set_address_packet(String packetBuffer) {
    long new_addr = packetBuffer.substring(SET_ADDRESS_ADDR_START, SET_ADDRESS_ADDR_END).toInt();

    if (debug) {
        Serial.printf("Set address packet received.\r\n\tNew Device addr : %03ld\r\n", new_addr);
    }

    modem_id = new_addr;
}

void parse_broadcast_packet(String packetBuffer) {
    long src_addr = packetBuffer.substring(BROADCAST_SRC_ADDR_START, BROADCAST_SRC_ADDR_END).toInt();
    long bytes = packetBuffer.substring(BROADCAST_BYTE_LENGTH_START, BROADCAST_BYTE_LENGTH_END).toInt();

    String packetData = packetBuffer.substring(BROADCAST_PACKET_DATA_START, BROADCAST_PACKET_DATA_START + bytes);

    if (debug) {
        Serial.printf("Broadcast packet received.\r\n\tPacket src addr : %03ld\r\n\tPacket Bytes : %ld\r\n\tPacket data : ", src_addr, bytes);
        Serial.print(packetData + "\r\n");
    }

    char *broadcastBuffer = (char*)malloc(PACKET_BUFFER_SIZE);
    packetData.toCharArray(broadcastBuffer, PACKET_BUFFER_SIZE);
    floc_broadcast_received(broadcastBuffer, bytes);
    free(broadcastBuffer);
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
void parse_ping_packet(String packetBuffer) {
    long ping_addr = packetBuffer.substring(PING_ADDR_START, PING_ADDR_END).toInt();
    long ping_propogation_counter = packetBuffer.substring(PING_PROPOGATION_COUNTER_START, PING_PROPOGATION_COUNTER_END).toInt();
    float meter_range = static_cast<float>(ping_propogation_counter) * SOUND_SPEED * 3.125e-5;

    if (debug) {
        Serial.printf("Ping (or ACK) packet received.\r\n\tAddr : %03ld\r\n\tRange (m) : %f\r\n", ping_addr, meter_range);
    }
}

void packet_recieved(String packetBuffer) {
    // Serial.printf("Packet length %d\nPacket data : ", packetBuffer.length());
    // Serial.print(packetBuffer + "\n");
    
    if (packetBuffer.length() < 1) {
        // Should never happen over serial connection.
        return;
    }

    if (packetBuffer.charAt(0) == 'E') {
        if (debug) Serial.printf("Error packet received.\r\n");
        return;
    }

    if (packetBuffer.charAt(0) == '$') {
        // Local Echo
        switch (packetBuffer.charAt(1)) {
            case '?':
                break;
            case 'A':
                break;
            case 'B':
                if (packetBuffer.length() == BROADCAST_LOCAL_ECHO_LENGTH) {
                    if (debug) {
                        Serial.printf("Broadcast of %02ld bytes sent.\r\n", 
                        packetBuffer.substring(BROADCAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                                               BROADCAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt());
                    }
                }
                break;
            case 'M':
                if (packetBuffer.length() == UNICAST_LOCAL_ECHO_LENGTH) {
                    if (debug) {
                        Serial.printf("Unicast (with ACK) of %02ld bytes to address %03ld sent.\r\n",
                        packetBuffer.substring(UNICAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                                               UNICAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt(),
                        packetBuffer.substring(UNICAST_LOCAL_ECHO_DEST_ADDR_START, 
                                               UNICAST_LOCAL_ECHO_DEST_ADDR_END).toInt());
                    }

                }
                break;
            case 'P':
                if (packetBuffer.length() == PING_LOCAL_ECHO_LENGTH) {
                    if (debug) {
                        Serial.printf("Ping to modem %03ld sent.\r\n", 
                        packetBuffer.substring(PING_LOCAL_ECHO_DEST_ADDR_START, 
                                               PING_LOCAL_ECHO_DEST_ADDR_END).toInt());
                    }
                }
                break;
            case 'U':
                if (packetBuffer.length() == UNICAST_LOCAL_ECHO_LENGTH) {
                    if (debug) {
                        Serial.printf("Unicast of %02ld bytes to address %03ld sent.\r\n",
                        packetBuffer.substring(UNICAST_LOCAL_ECHO_BYTE_LENGTH_START, 
                                               UNICAST_LOCAL_ECHO_BYTE_LENGTH_END).toInt(),
                        packetBuffer.substring(UNICAST_LOCAL_ECHO_DEST_ADDR_START, 
                                               UNICAST_LOCAL_ECHO_DEST_ADDR_END).toInt());
                    }

                }
                break;
            case 'V':
                break;

            default:
                Serial.printf("Unhandled packet type : prefix [%c]\r\n", packetBuffer.charAt(1));
                Serial.println("Full packet : " + packetBuffer);
        }
    } else if (packetBuffer.charAt(0) == '#') {
        switch (packetBuffer.charAt(1)) {
            case '?':
                break;
            case 'A':
                if (packetBuffer.length() == STATUS_QUERY_PACKET_LENGTH) {
                    parse_status_query_packet(packetBuffer);
                } else if (packetBuffer.length() == SET_ADDRESS_PACKET_LENGTH) {
                    parse_set_address_packet(packetBuffer);
                }
                break;
            case 'B':
                    parse_broadcast_packet(packetBuffer);
                break;
            case 'M':
                // TODO : Handle unicast with ack? Does this exist with # prefix?
                break;
            case 'P':
                
                break;
            case 'R':
                if (packetBuffer.length() == PING_PACKET_LENGTH) {
                    parse_ping_packet(packetBuffer);
                }
                break;
            case 'T':
                if (packetBuffer.length() == TIMEOUT_PACKET_LENGTH) {
                    if (debug) {
                        Serial.println("Timeout.");
                    }
                }
                break;
            case 'U':
                    parse_unicast_packet(packetBuffer);
                break;
            case 'V':
                // TODO : Handling will require modifying Broadcast packet recv handling
                break;

            default:
                Serial.printf("Unhandled packet type.\n\tPrefix : %c\r\n", packetBuffer.charAt(1));
                Serial.println("\tFull packet : " + packetBuffer);
        }
    } else {
        // Packet does not follow modem response structure (start with $ or #)
        if (debug) print_packet(packetBuffer, "Unknown prefix");
        return;
    }
}