#include "floc.hpp"


uint8_t packet_id = 0;
int status_response_dest_addr = -1, status_request_pid = -1; // Address that has requested modem status info

void parse_floc_command_packet(char *broadcastBuffer, uint8_t size) {
    if (debug) Serial.println("Command packet received");

    struct command_header *header = (struct command_header *)broadcastBuffer;

    // Code to release buoy goes here

    floc_acknowledgement_send(Serial1, header->src_addr, header->pid);
}

void parse_floc_acknowledgement_packet(char *broadcastBuffer) {
    if (debug) Serial.println("Acknowledgement packet received");

    struct ack_header *header = (struct ack_header *)broadcastBuffer;

}

void parse_floc_response_packet(char *broadcastBuffer) {
    if (debug) Serial.println("Response packet received");

    struct response_header *header = (struct response_header *)broadcastBuffer;

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
    // May not be used
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

void floc_status_queue(HardwareSerial connection, uint8_t dest_addr) {
    status_response_dest_addr = dest_addr;
    query_status(connection);
}

// TODO : Is there a way to dynamically send the HardwareSerial structure to this function?
void floc_status_send(String status) {
    if (status_response_dest_addr < 0) {
        return; // No requests for this modem's status have been received
    }

    int status_packet_size = sizeof(struct response_header) + status.length();
    struct response_header *response = (struct response_header *)malloc(status_packet_size);

    response->ttl = TTL_START; 
    response->type = RESPONSE_TYPE;
    response->dest_addr = status_response_dest_addr;
    response->src_addr = get_modem_address();
    response->pid = packet_id++;
    response->request_pid = status_request_pid;
    response->size = status_packet_size;

    status_response_dest_addr = -1; // Clear status request

    char *broadcastPacket = (char *)response;
    char *status_start = broadcastPacket + sizeof(response_header);
    status.toCharArray(status_start, status.length());

    broadcast(Serial1, broadcastPacket, status_packet_size);
}