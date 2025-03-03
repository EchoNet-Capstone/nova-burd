#include <iostream>
#include <cstdio>
#include <cstring>
#include "floc.hpp"
#include "nmv3_api.hpp"

// Helper function to print a buffer in hexadecimal.
void printBytes(const unsigned char* buffer, size_t len, const char* label) {
    std::cout << label << std::endl;
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buffer[i]);
    }
    std::cout << "\n" << std::endl;
}

//=============================
// FLOC Packet Test Functions
//=============================

void testFLOCCommandPacket() {
    std::cout << "=== FLOC Command Packet ===" << std::endl;
    FlocPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    // Fill FlocHeader_t (bitfields)
    pkt.header.ttl = 0xB;                    // 4-bit TTL
    pkt.header.type = FLOC_COMMAND_TYPE;     // 0x1 for command packets
    pkt.header.nid = 0x1234;
    pkt.header.res = 0;
    pkt.header.pid = 0x2;
    pkt.header.dest_addr = 0x5678;
    pkt.header.src_addr = 0x9ABC;

    // Fill CommandPacket_t header and data.
    pkt.payload.command.header.command_type = COMMAND_TYPE_1;
    pkt.payload.command.header.size = 3;     // Command data size
    pkt.payload.command.data[0] = 'A';
    pkt.payload.command.data[1] = 'S';
    pkt.payload.command.data[2] = 'D';

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "FLOC Command Packet:");
}

void testFLOCDataPacket() {
    std::cout << "=== FLOC Data Packet ===" << std::endl;
    FlocPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.header.ttl = 0xA;
    pkt.header.type = FLOC_DATA_TYPE;        // Data packet type
    pkt.header.nid = 0x1111;
    pkt.header.res = 0;
    pkt.header.pid = 0x3;
    pkt.header.dest_addr = 0x2222;
    pkt.header.src_addr = 0x3333;

    // Fill DataPacket_t header and sample data (5 bytes)
    pkt.payload.data.header.size = 5;
    for (int i = 0; i < 5; i++) {
        pkt.payload.data.data[i] = static_cast<uint8_t>('0' + i);
    }

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "FLOC Data Packet:");
}

void testFLOCAckPacket() {
    std::cout << "=== FLOC Ack Packet ===" << std::endl;
    FlocPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.header.ttl = 0xF;
    pkt.header.type = FLOC_ACK_TYPE;         // Ack packet type
    pkt.header.nid = 0x4444;
    pkt.header.res = 0;
    pkt.header.pid = 0x1;
    pkt.header.dest_addr = 0x5555;
    pkt.header.src_addr = 0x6666;

    pkt.payload.ack.header.ack_pid = 0x7;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "FLOC Ack Packet:");
}

void testFLOCResponsePacket() {
    std::cout << "=== FLOC Response Packet ===" << std::endl;
    FlocPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.header.ttl = 0xC;
    pkt.header.type = FLOC_RESPONSE_TYPE;    // Response packet type
    pkt.header.nid = 0x7777;
    pkt.header.res = 0;
    pkt.header.pid = 0x4;
    pkt.header.dest_addr = 0x8888;
    pkt.header.src_addr = 0x9999;

    pkt.payload.response.header.request_pid = 0x2;
    pkt.payload.response.header.size = 4;
    for (int i = 0; i < 4; i++) {
        pkt.payload.response.data[i] = static_cast<uint8_t>('A' + i);
    }

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "FLOC Response Packet:");
}

void testSerialFLOCPacket() {
    std::cout << "=== Serial FLOC Packet ===" << std::endl;
    SerialFlocPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.header.type = SERIAL_BROADCAST_TYPE; // For example, broadcast type
    pkt.header.size = 10;                    // Example size

    // Using the broadcast variant – fill a FLOC command inside the serial packet.
    pkt.payload.broadcast.floc_packet.command.header.command_type = COMMAND_TYPE_1;
    pkt.payload.broadcast.floc_packet.command.header.size = 3;
    pkt.payload.broadcast.floc_packet.command.data[0] = 'X';
    pkt.payload.broadcast.floc_packet.command.data[1] = 'Y';
    pkt.payload.broadcast.floc_packet.command.data[2] = 'Z';

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Serial FLOC Packet:");
}

//=========================================
// Heltec Serial API – Command Packets Tests
//=========================================

void testModemCommandPacket_SetAddress() {
    std::cout << "=== Modem Command Packet: Set Address ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = SET_ADDRESS_CMD_TYPE; // 'A'
    pkt.command.setAddress.addr[0] = 0x10;
    pkt.command.setAddress.addr[1] = 0x20;
    pkt.command.setAddress.addr[2] = 0x30;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Set Address Packet:");
}

void testModemCommandPacket_BroadcastMessage() {
    std::cout << "=== Modem Command Packet: Broadcast Message ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = BROADCAST_CMD_TYPE; // 'B'
    // Fill Broadcast Message header (dataSize is an array)
    pkt.command.broadcastMessage.header.dataSize[0] = 0x03;
    pkt.command.broadcastMessage.header.dataSize[1] = 0x00;
    // Fill payload with sample data
    pkt.command.broadcastMessage.payload[0] = 'H';
    pkt.command.broadcastMessage.payload[1] = 'I';
    pkt.command.broadcastMessage.payload[2] = '!';
    
    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Broadcast Message Packet:");
}

void testModemCommandPacket_ChannelImpulse() {
    std::cout << "=== Modem Command Packet: Channel Impulse ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = CHN_IMP_CMD_TYPE; // 'C'
    pkt.command.channelImpulse.magnitudeComplex = 0x5A;
    pkt.command.channelImpulse.addr[0] = 0x01;
    pkt.command.channelImpulse.addr[1] = 0x02;
    pkt.command.channelImpulse.addr[2] = 0x03;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Channel Impulse Packet:");
}

void testModemCommandPacket_EchoMessage() {
    std::cout << "=== Modem Command Packet: Echo Message ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = ECHO_MSG_CMD_TYPE; // 'E'
    // Fill Echo Message header: address (3 bytes) and dataSize (2 bytes)
    pkt.command.echoMessage.header.addr[0] = 0xAA;
    pkt.command.echoMessage.header.addr[1] = 0xBB;
    pkt.command.echoMessage.header.addr[2] = 0xCC;
    pkt.command.echoMessage.header.dataSize[0] = 2;
    pkt.command.echoMessage.header.dataSize[1] = 0;
    // Fill payload with sample text
    pkt.command.echoMessage.payload[0] = 'O';
    pkt.command.echoMessage.payload[1] = 'K';

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Echo Message Packet:");
}

void testModemCommandPacket_UnicastWithAck() {
    std::cout << "=== Modem Command Packet: Unicast With Ack ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = UNICAST_ACK_CMD_TYPE; // 'M'
    pkt.command.unicastWithAck.header.addr[0] = 0x11;
    pkt.command.unicastWithAck.header.addr[1] = 0x22;
    pkt.command.unicastWithAck.header.addr[2] = 0x33;
    pkt.command.unicastWithAck.header.dataSize[0] = 1;
    pkt.command.unicastWithAck.header.dataSize[1] = 0;
    pkt.command.unicastWithAck.payload[0] = 'U';

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Unicast With Ack Packet:");
}

void testModemCommandPacket_Ping() {
    std::cout << "=== Modem Command Packet: Ping ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = PING_CMD_TYPE; // 'P'
    pkt.command.ping.addr[0] = 0x01;
    pkt.command.ping.addr[1] = 0x02;
    pkt.command.ping.addr[2] = 0x03;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Ping Packet:");
}

void testModemCommandPacket_TestMsg() {
    std::cout << "=== Modem Command Packet: Test Message ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = TEST_MSG_CMD_TYPE; // 'T'
    pkt.command.testMsg.addr[0] = 0xDE;
    pkt.command.testMsg.addr[1] = 0xAD;
    pkt.command.testMsg.addr[2] = 0xBE;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Test Message Packet:");
}

void testModemCommandPacket_Unicast() {
    std::cout << "=== Modem Command Packet: Unicast ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = UNICAST_MSG_CMD_TYPE; // 'U'
    pkt.command.unicast.header.addr[0] = 0x05;
    pkt.command.unicast.header.addr[1] = 0x06;
    pkt.command.unicast.header.addr[2] = 0x07;
    pkt.command.unicast.header.dataSize[0] = 2;
    pkt.command.unicast.header.dataSize[1] = 0;
    pkt.command.unicast.payload[0] = 'O';
    pkt.command.unicast.payload[1] = 'K';

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Unicast Packet:");
}

void testModemCommandPacket_VoltageAndNoise() {
    std::cout << "=== Modem Command Packet: Voltage and Noise ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = VOLT_NOISE_MSR_CMD_TYPE; // 'V'
    pkt.command.voltageAndNoise.addr[0] = 0x0A;
    pkt.command.voltageAndNoise.addr[1] = 0x0B;
    pkt.command.voltageAndNoise.addr[2] = 0x0C;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Voltage and Noise Packet:");
}

void testModemCommandPacket_Extension_SystemTime() {
    std::cout << "=== Modem Command Packet: Extension (System Time) ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = EXTEN_CMD_TYPE; // 'X'
    // Fill extension command packet with system time command.
    pkt.command.extPacket.type = SYS_TIME_EXT_CMD_TYPE; // 'T'
    // (SystemTimeCommandPacket_t has no additional payload in this example.)

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Extension Command (System Time) Packet:");
}

void testModemCommandPacket_Extension_LinkQuality() {
    std::cout << "=== Modem Command Packet: Extension (Link Quality) ===" << std::endl;
    ModemCommandPacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = EXTEN_CMD_TYPE; // 'X'
    pkt.command.extPacket.type = LINK_QUAL_EXT_CMD_TYPE; // 'Q'

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Extension Command (Link Quality) Packet:");
}

//=========================================
// Heltec Serial API – Local Response Tests
//=========================================

void testModemLocalResponsePacket_Broadcast() {
    std::cout << "=== Modem Local Response Packet: Broadcast ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = BROADCAST_CMD_LOCAL_RESP_TYPE; // 'B'
    pkt.response.broadcast.dataSize[0] = 2;
    pkt.response.broadcast.dataSize[1] = 0;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Broadcast) Packet:");
}

void testModemLocalResponsePacket_ChannelImpulse() {
    std::cout << "=== Modem Local Response Packet: Channel Impulse ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = CHN_IMP_CMD_LOCAL_RESP_TYPE; // 'C'
    pkt.response.channelImpulse.magnitudeComplex = 0x55;
    pkt.response.channelImpulse.addr[0] = 0x01;
    pkt.response.channelImpulse.addr[1] = 0x02;
    pkt.response.channelImpulse.addr[2] = 0x03;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Channel Impulse) Packet:");
}

void testModemLocalResponsePacket_EchoMessage() {
    std::cout << "=== Modem Local Response Packet: Echo Message ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = ECHO_MSG_CMD_LOCAL_RESP_TYPE; // 'E'
    pkt.response.echoMessage.addr[0] = 0xAA;
    pkt.response.echoMessage.addr[1] = 0xBB;
    pkt.response.echoMessage.addr[2] = 0xCC;
    pkt.response.echoMessage.dataSize[0] = 2;
    pkt.response.echoMessage.dataSize[1] = 0;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Echo) Packet:");
}

void testModemLocalResponsePacket_UnicastWithAck() {
    std::cout << "=== Modem Local Response Packet: Unicast With Ack ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = UNICAST_ACK_CMD_LOCAL_RESP_TYPE; // 'M'
    pkt.response.unicastWithAck.addr[0] = 0x11;
    pkt.response.unicastWithAck.addr[1] = 0x22;
    pkt.response.unicastWithAck.addr[2] = 0x33;
    pkt.response.unicastWithAck.dataSize[0] = 1;
    pkt.response.unicastWithAck.dataSize[1] = 0;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Unicast With Ack) Packet:");
}

void testModemLocalResponsePacket_Ping() {
    std::cout << "=== Modem Local Response Packet: Ping ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = PING_CMD_LOCAL_RESP_TYPE; // 'P'
    pkt.response.ping.addr[0] = 0x01;
    pkt.response.ping.addr[1] = 0x02;
    pkt.response.ping.addr[2] = 0x03;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Ping) Packet:");
}

void testModemLocalResponsePacket_CorrectedErrors() {
    std::cout << "=== Modem Local Response Packet: Corrected Errors ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = CORR_ERR_CMD_LOCAL_RESP_TYPE; // 'C'
    pkt.response.correctedErorrs.rS = 0x0F;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Corrected Errors) Packet:");
}

void testModemLocalResponsePacket_TestMessage() {
    std::cout << "=== Modem Local Response Packet: Test Message ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = TEST_MSG_CMD_LOCAL_RESP_TYPE; // 'T'
    pkt.response.testMessage.addr[0] = 0xDE;
    pkt.response.testMessage.addr[1] = 0xAD;
    pkt.response.testMessage.addr[2] = 0xBE;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Test Message) Packet:");
}

void testModemLocalResponsePacket_Unicast() {
    std::cout << "=== Modem Local Response Packet: Unicast ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = UNICAST_CMD_LOCAL_RESP_TYPE; // 'U'
    pkt.response.unicast.addr[0] = 0x05;
    pkt.response.unicast.addr[1] = 0x06;
    pkt.response.unicast.addr[2] = 0x07;
    pkt.response.unicast.dataSize[0] = 2;
    pkt.response.unicast.dataSize[1] = 0;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Unicast) Packet:");
}

void testModemLocalResponsePacket_VoltageAndNoise() {
    std::cout << "=== Modem Local Response Packet: Voltage and Noise ===" << std::endl;
    ModemLocalResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = VOLT_NOISE_MSR_CMD_LOCAL_RESP_TYPE; // 'V'
    pkt.response.voltageAndNoise.addr[0] = 0x0A;
    pkt.response.voltageAndNoise.addr[1] = 0x0B;
    pkt.response.voltageAndNoise.addr[2] = 0x0C;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Local Response (Voltage and Noise) Packet:");
}

//=========================================
// Heltec Serial API – Response Packet Tests
//=========================================

void testModemResponsePacket_QueryStatus() {
    std::cout << "=== Modem Response Packet: Query Status ===" << std::endl;
    ModemResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = QUERY_STATUS_RESP_TYPE; // 'A'
    // Use the fullStatus variant of QueryStatusResponsePacket_t.
    pkt.response.queryStatus.status.fullStatus.addr[0] = 0x01;
    pkt.response.queryStatus.status.fullStatus.addr[1] = 0x02;
    pkt.response.queryStatus.status.fullStatus.addr[2] = 0x03;
    pkt.response.queryStatus.status.fullStatus.voltSep = 'V';
    for (int i = 0; i < BROADCAST_RESP_PAYLOAD_MAX; i++) {
        pkt.response.queryStatus.status.fullStatus.voltPayload[i] = i;
    }
    pkt.response.queryStatus.status.fullStatus.releaseSep = 'R';
    for (int i = 0; i < QUERY_STATUS_RESP_REL_PAYLOAD_MAX; i++) {
        pkt.response.queryStatus.status.fullStatus.releasePayload[i] = i + 10;
    }
    pkt.response.queryStatus.status.fullStatus.buildTimeSep = 'B';
    for (int i = 0; i < QUERY_STATUS_RESP_BT_YEAR_PAYLOAD_MAX; i++) {
        pkt.response.queryStatus.status.fullStatus.buildTime.buildTimeYearPayload[i] = i + 20;
    }
    pkt.response.queryStatus.status.fullStatus.buildTime.buildTimeTimeSep = 'T';
    for (int i = 0; i < QUERY_STATUS_RESP_BT_TIME_PAYLOAD_MAX; i++) {
        pkt.response.queryStatus.status.fullStatus.buildTime.buildTimeTimePayload[i] = i + 30;
    }
    // Extra data (using link quality extra as an example)
    pkt.extraData.data.linkQuality.lqQualSep = 'Q';
    for (int i = 0; i < RESP_EXTRA_LQ_QUAL_PAYLOAD_MAX; i++) {
        pkt.extraData.data.linkQuality.lqQualPayload[i] = i;
    }
    pkt.extraData.data.linkQuality.lqDoppSep = 'D';
    for (int i = 0; i < RESP_EXTRA_LQ_DOPP_PAYLOAD_MAX; i++) {
        pkt.extraData.data.linkQuality.lqDoppPayload[i] = i + 5;
    }

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Response Packet (Query Status):");
}

void testModemResponsePacket_Broadcast() {
    std::cout << "=== Modem Response Packet: Broadcast ===" << std::endl;
    ModemResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = BROADCAST_RESP_TYPE; // 'B'
    pkt.response.broadcast.header.addr[0] = 0x11;
    pkt.response.broadcast.header.addr[1] = 0x22;
    pkt.response.broadcast.header.addr[2] = 0x33;
    pkt.response.broadcast.header.dataSize[0] = 3;
    pkt.response.broadcast.header.dataSize[1] = 0;
    pkt.response.broadcast.message.payload[0] = 'H';
    pkt.response.broadcast.message.payload[1] = 'I';
    pkt.response.broadcast.message.payload[2] = '!';

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Response Packet (Broadcast):");
}

void testModemResponsePacket_RangeData() {
    std::cout << "=== Modem Response Packet: Range Data ===" << std::endl;
    ModemResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = RANGE_RESP_TYPE; // 'R'
    pkt.response.rangeData.addr[0] = 0x01;
    pkt.response.rangeData.addr[1] = 0x02;
    pkt.response.rangeData.addr[2] = 0x03;
    pkt.response.rangeData.rangeSep = 'T';
    for (int i = 0; i < RANGE_RESP_PAYLOAD_MAX; i++) {
        pkt.response.rangeData.rangePayload[i] = i + 1;
    }

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Response Packet (Range Data):");
}

void testModemResponsePacket_Unicast() {
    std::cout << "=== Modem Response Packet: Unicast ===" << std::endl;
    ModemResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.type = UNICAST_RESP_TYPE; // 'U'
    pkt.response.unicast.dataSize[0] = 2;
    pkt.response.unicast.dataSize[1] = 0;
    pkt.response.unicast.payload[0] = 'O';
    pkt.response.unicast.payload[1] = 'K';

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Response Packet (Unicast):");
}

void testModemResponsePacket_Extension_SystemTime() {
    std::cout << "=== Modem Response Packet: Extension (System Time) ===" << std::endl;
    ModemResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    // For an extension response, fill the extPacket.
    pkt.response.extPacket.type = SYS_TIME_EXT_RESP_TYPE; // 'T'
    pkt.response.extPacket.response.systemTime.status = 0x01;
    for (int i = 0; i < SYS_TIME_EXT_RESP_PAYLOAD_MAX; i++) {
        pkt.response.extPacket.response.systemTime.timePayload[i] = i + 10;
    }

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Response Packet (Extension - System Time):");
}

void testModemResponsePacket_Extension_LinkQuality() {
    std::cout << "=== Modem Response Packet: Extension (Link Quality) ===" << std::endl;
    ModemResponsePacket_t pkt;
    std::memset(&pkt, 0, sizeof(pkt));

    pkt.response.extPacket.type = LINK_QUAL_EXT_RESP_TYPE; // 'Q'
    pkt.response.extPacket.response.linkQuality.status = 0x02;

    printBytes(reinterpret_cast<const unsigned char*>(&pkt), sizeof(pkt),
               "Modem Response Packet (Extension - Link Quality):");
}

//=============================
// Main – Run All Tests
//=============================

int main() {
    // FLOC packet tests
    testFLOCCommandPacket();
    testFLOCDataPacket();
    testFLOCAckPacket();
    testFLOCResponsePacket();
    testSerialFLOCPacket();

    // Heltec Serial API – Command Packets
    testModemCommandPacket_SetAddress();
    testModemCommandPacket_BroadcastMessage();
    testModemCommandPacket_ChannelImpulse();
    testModemCommandPacket_EchoMessage();
    testModemCommandPacket_UnicastWithAck();
    testModemCommandPacket_Ping();
    testModemCommandPacket_TestMsg();
    testModemCommandPacket_Unicast();
    testModemCommandPacket_VoltageAndNoise();
    testModemCommandPacket_Extension_SystemTime();
    testModemCommandPacket_Extension_LinkQuality();

    // Heltec Serial API – Local Response Packets
    testModemLocalResponsePacket_Broadcast();
    testModemLocalResponsePacket_ChannelImpulse();
    testModemLocalResponsePacket_EchoMessage();
    testModemLocalResponsePacket_UnicastWithAck();
    testModemLocalResponsePacket_Ping();
    testModemLocalResponsePacket_CorrectedErrors();
    testModemLocalResponsePacket_TestMessage();
    testModemLocalResponsePacket_Unicast();
    testModemLocalResponsePacket_VoltageAndNoise();

    // Heltec Serial API – Response Packets
    testModemResponsePacket_QueryStatus();
    testModemResponsePacket_Broadcast();
    testModemResponsePacket_RangeData();
    testModemResponsePacket_Unicast();
    testModemResponsePacket_Extension_SystemTime();
    testModemResponsePacket_Extension_LinkQuality();

    return 0;
}
