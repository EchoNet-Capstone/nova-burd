#include "floc.hpp"


/*

# for packet id, the combination of PID, source, and type should be identical. 
This way, response / acknowledgement packets can have the same PID field as their corresponding command packets.

*/

uint8_t packet_id = 0;

void parse_floc_command_packet(char *broadcastBuffer, uint8_t size) {
    if (debug) Serial.println("Command packet received");

    struct command_header *header = (struct command_header *)broadcastBuffer;

    // Code to release buoy goes here

    floc_acknowledgement_send(Serial1, header->src_addr, header->pid);
}

void parse_floc_acknowledgement_packet(char *broadcastBuffer) {
    if (debug) Serial.println("Acknowledgement packet received");

}

void parse_floc_response_packet(char *broadcastBuffer) {
    if (debug) Serial.println("Response packet received");

}

void floc_broadcast_received(char *broadcastBuffer, uint8_t size) {

    struct header_common *common = (struct header_common *)broadcastBuffer;
    
    int8_t type = common->type;
    printf("Type is %d\r\n", type);

    if (common->dest_addr == get_modem_address()) {
        // The packet is addressed to this node

        switch (type) {
            case COMMAND_TYPE:
                parse_floc_command_packet(broadcastBuffer, size);
                break;
            case ACK_TYPE:
                parse_floc_acknowledgement_packet(broadcastBuffer);
                break;
            case RESPONSE_TYPE:
                parse_floc_response_packet(broadcastBuffer);
                break;
        }
    } else {
        // Packet is not addressed to this node

        // TODO : Mesh network rebroadcasting to be handled here
    }

}

void floc_unicast_received(char *unicastBuffer, uint8_t size) {

}

void floc_acknowledgement_send(HardwareSerial connection, uint8_t dest_addr, uint8_t ack_pid) {

    struct ack_header *acknowledgement = (struct ack_header *)malloc(sizeof(struct ack_header));

    acknowledgement->ttl = TTL_START; 
    acknowledgement->type = ACK_TYPE;
    acknowledgement->dest_addr = dest_addr;
    acknowledgement->src_addr = get_modem_address();
    acknowledgement->pid = packet_id++;
    acknowledgement->ack_pid = ack_pid;

    char *broadcastPacket = (char *)acknowledgement;

    //Serial.print(broadcastPacket);
    broadcast(connection, broadcastPacket, sizeof(struct ack_header));
}