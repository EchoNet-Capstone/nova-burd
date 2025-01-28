#include "packet_handler.hpp"



void print_packet(String packetBuffer, String prefix) {
    Serial.println(prefix + packetBuffer);
}

void query_status(HardwareSerial connection) {
    connection.print("$?");
}

void set_address(HardwareSerial connection, int8_t addr){
    connection.printf("$A%03d", addr);
}

void broadcast(HardwareSerial connection, char *data, int8_t bytes){
    connection.printf("$B%02d%s", bytes, data);
}

void packet_recieved(String packetBuffer) {
    
    if (packetBuffer.length() < 1) {
        // Packet not formatted correctly
        return;
    }

    if (packetBuffer.charAt(0) == 'E') {
        // Error packet
        return;
    }
    
    if (packetBuffer.charAt(0) != '$' && packetBuffer.charAt(0) != '#') {
        // Packet does not follow modem response structure
        return;
    }

    switch (packetBuffer.charAt(1)) {
        case '?':
            // Status query
            if (debug) print_packet(packetBuffer, "Status query received, full packet : ");
            break;
        case 'A':
            // Set address of modem
            if (debug) print_packet(packetBuffer, "Address reset received, full packet : ");
            break;
        case 'B':
            // Broadcast
            if (debug) print_packet(packetBuffer, "Broadcast packet received, full packet : ");
            break;
        case 'M':
            // Unicast with acknowledgement
            if (debug) print_packet(packetBuffer, "Unicast (with ACK) packet received, full packet : ");
            break;
        case 'P':
            // Ping (for ranging data)
            if (debug) print_packet(packetBuffer, "Ping packet received, full packet : ");
            break;
        case 'U':
            // Unicast without acknowledgement
            if (debug) print_packet(packetBuffer, "Unicast packet received, full packet : ");
            break;
        case 'V':
            // Battery voltage and noise measurement of modem
            if (debug) print_packet(packetBuffer, "Batt Volt + Noise Measurement packet received, full packet : ");
            break;

        default:
            Serial.printf("Unhandled packet type : prefix [%c]\r\n", packetBuffer.charAt(1));
            Serial.println("Full packet : " + packetBuffer);
    }
}