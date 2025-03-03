#ifndef HELTEC_SERIAL_API_H
#define HELTEC_SERIAL_API_H

#include "Arduino.h"
#include "globals.hpp"
#include "display.hpp"

// --- Configuration (Maximum Sizes) ---
#define MODEM_DATA_MIN 2
#define MODEM_PAYLOAD_MAX 64

// --- Error Packet ---
#define ERROR_PRE_MAX 1
#define ERROR_MAX 0

// --- Commands ---
#define MODEM_COMMAND_PRE_MAX     	1

#define MODEM_COMMAND_TYPE_MAX	  	1

// Query Status
#define QUERY_STATUS_CMD_MAX      	0

// Set Address
#define SET_ADDRESS_CMD_ADDR_MAX    3
#define SET_ADDRESS_CMD_MAX       	SET_ADDRESS_CMD_ADDR_MAX

// Broadcast Message
#define BROADCAST_CMD_DATA_SIZE_MAX 2
#define BROADCAST_CMD_HDR_MAX     	BROADCAST_CMD_DATA_SIZE_MAX
#define BROADCAST_CMD_DATA_MIN    	MODEM_DATA_MIN
#define BROADCAST_CMD_PAYLOAD_MAX   MODEM_PAYLOAD_MAX
#define BROADCAST_CMD_MIN         	BROADCAST_CMD_HDR_MAX + \
                                    BROADCAST_CMD_DATA_MIN
#define BROADCAST_CMD_MAX         	BROADCAST_CMD_HDR_MAX + \
                                    BROADCAST_CMD_PAYLOAD_MAX

// Channel Impulse 
#define CHN_IMP_CMD_MAG_COMP_MAX    1
#define CHN_IMP_CMD_ADDR_MAX        3
#define CHN_IMP_CMD_MAX           	CHN_IMP_CMD_MAG_COMP_MAX + \
                                    CHN_IMP_CMD_ADDR_MAX

// Echo Message
#define ECHO_MSG_CMD_ADDR_MAX       3
#define ECHO_MSG_CMD_DATA_SIZE_MAX  2
#define ECHO_MSG_CMD_HDR_MAX      	ECHO_MSG_CMD_ADDR_MAX + \
                                    ECHO_MSG_CMD_HDR_MAX
#define ECHO_MSG_CMD_DATA_MIN     	MODEM_DATA_MIN
#define ECHO_MSG_CMD_PAYLOAD_MAX    MODEM_PAYLOAD_MAX
#define ECHO_MSG_CMD_MIN          	ECHO_MSG_CMD_HDR_MAX + \
                                    ECHO_MSG_CMD_DATA_MIN
#define ECHO_MSG_CMD_MAX          	ECHO_MSG_CMD_HDR_MAX + \
                                    ECHO_MSG_CMD_PAYLOAD_MAX

// Help
#define HELP_CMD_MAX                    0

// Unicast with Ack Message
#define UNICAST_ACK_CMD_ADDR_MAX        3
#define UNICAST_ACK_CMD_DATA_SIZE_MAX   2
#define UNICAST_ACK_CMD_HDR_MAX      	  UNICAST_ACK_CMD_ADDR_MAX + \
                                        UNICAST_ACK_CMD_HDR_MAX
#define UNICAST_ACK_CMD_DATA_MIN  	    MODEM_DATA_MIN
#define UNICAST_ACK_CMD_PAYLOAD_MAX  	  MODEM_PAYLOAD_MAX
#define UNICAST_ACK_CMD_MIN       	    UNICAST_ACK_CMD_HDR_MAX + \
                                        UNICAST_ACK_CMD_DATA_MIN
#define UNICAST_ACK_CMD_MAX       	    UNICAST_ACK_CMD_HDR_MAX + \
                                        UNICAST_ACK_CMD_PAYLOAD_MAX

// Noise Measurement
#define NOISE_MSR_CMD_MAX         	0

// Ping
#define PING_CMD_ADDR_MAX           3
#define PING_CMD_MAX              	PING_CMD_ADDR_MAX

// Corrected Errors
#define CORR_ERR_CMD_MAX          	0

// Reset
#define RESET_CMD_MAX             	0

// Spectrun Measurement
#define SPEC_MSR_CMD_MAX          	0

// Test
#define TEST_MSG_CMD_ADDR_MAX       3
#define TEST_MSG_CMD_MAX          	TEST_MSG_CMD_ADDR_MAX

// Unicast Message
#define UNICAST_CMD_ADDR_MAX        3
#define UNICAST_CMD_DATA_SIZE_MAX   2
#define UNICAST_CMD_HDR_MAX       	UNICAST_CMD_ADDR_MAX + \
                                    UNICAST_CMD_DATA_SIZE_MAX +
#define UNICAST_CMD_DATA_MIN      	MODEM_DATA_MIN
#define UNICAST_CMD_PAYLOAD_MAX     MODEM_PAYLOAD_MAX
#define UNICAST_CMD_MIN           	UNICAST_CMD_HDR_MAX + \
                                    UNICAST_CMD_DATA_MIN
#define UNICAST_CMD_MAX           	UNICAST_CMD_HDR_MAX + \
                                    UNICAST_CMD_PAYLOAD_MAX

// Voltage and Noise Measurement
#define VOLT_NOISE_MSR_CMD_ADDR_MAX 3
#define VOLT_NOISE_MSR_CMD_MAX      VOLT_NOISE_MSR_CMD_ADDR_MAX

// Extenion Commands
#define EXTEN_CMD_TYPE_MAX          1
#define EXTEN_CMD_MAX             	EXTEN_CMD_TYPE_MAX

// --- Extension Commands ---
#define SYS_TIME_EXT_CMD_TYPE_MAX   1
#define SYS_TIME_EXT_CMD_MAX        SYS_TIME_EXT_CMD_TYPE_MAX
#define LINK_QUAL_EXT_CMD_TYPE_MAX  1
#define LINK_QUAL_EXT_CMD_MAX       LINK_QUAL_EXT_CMD_TYPE_MAX

// --- Local Responses ---
#define MODEM_LOCAL_RESP_PRE_MAX        1

#define MODEM_LOCAL_RESP_TYPE_MAX       1

// Broadcast Message
#define BROADCAST_CMD_LOCAL_RESP_DATA_SIZE_MAX  2
#define BROADCAST_CMD_LOCAL_RESP_MAX            BROADCAST_CMD_LOCAL_RESP_DATA_SIZE_MAX

// Channel Impulse
#define CHN_IMP_CMD_LOCAL_RESP_MAG_COMP_MAX     1
#define CHN_IMP_CMD_LOCAL_RESP_ADDR_MAX         3
#define CHN_IMP_CMD_LOCAL_RESP_MAX           	  CHN_IMP_CMD_LOCAL_RESP_MAG_COMP_MAX + \
                                                CHN_IMP_CMD_LOCAL_RESP_ADDR_MAX

// Echo Message
#define ECHO_MSG_CMD_LOCAL_RESP_ADDR_MAX        3
#define ECHO_MSG_CMD_LOCAL_RESP_DATA_SIZE_MAX   2
#define ECHO_MSG_CMD_LOCAL_RESP_MAX      	      ECHO_MSG_CMD_LOCAL_RESP_ADDR_MAX + \
                                                ECHO_MSG_CMD_LOCAL_RESP_DATA_SIZE_MAX

// Unicast with Ack Message
#define UNICAST_ACK_CMD_LOCAL_RESP_ADDR_MAX       3
#define UNICAST_ACK_CMD_LOCAL_RESP_DATA_SIZE_MAX  2
#define UNICAST_ACK_CMD_LOCAL_RESP_MAX            UNICAST_ACK_CMD_LOCAL_RESP_ADDR_MAX + \
                                                  UNICAST_ACK_CMD_LOCAL_RESP_DATA_SIZE_MAX

// Ping
#define PING_CMD_LOCAL_RESP_ADDR_MAX  3
#define PING_CMD_LOACL_RESP_MAX       PING_CMD_LOCAL_RESP_ADDR_MAX

// Corrected Errors
#define CORR_ERR_CMD_LOCAL_RESP_RS_MAX  1
#define CORR_ERR_CMD_LOCAL_RESP_MAX     CORR_ERR_CMD_LOCAL_RESP_RS_MAX

// Reset
#define RESET_CMD_LOCAL_RESP_MAX        0

// Spectrum Measurement
#define SPEC_MSR_CMD_LOCAL_RESP_MAX     0

// Test
#define TEST_MSG_CMD_LOCAL_RESP_ADDR_MAX    3
#define TEST_MSG_CMD_LOCAL_RESP_MAX         TEST_MSG_CMD_LOCAL_RESP_ADDR_MAX

// Unicast Message
#define UNICAST_CMD_LOCAL_RESP_ADDR_MAX       3
#define UNICAST_CMD_LOCAL_RESP_DATA_SIZE_MAX  2
#define UNICAST_CMD_LOCAL_RESP_MAX            UNICAST_CMD_LOCAL_RESP_ADDR_MAX + \
                                              UNICAST_CMD_LOCAL_RESP_DATA_SIZE_MAX

// Voltage and Noise Measurement
#define VOLT_NOISE_MSR_CMD_LOCAL_RESP_ADDR_MAX  3
#define VOLT_NOISE_MSR_CMD_LOCAL_RESP_MAX       VOLT_NOISE_MSR_CMD_LOCAL_RESP_ADDR_MAX

// --- Responses ---
#define MODEM_RESP_PRE_MAX  1

#define MODEM_RESP_TYPE_MAX 1

// Response Extra Data
#define RESP_EXTRA_FIELD_PRE_MAX        1
#define RESP_EXTRA_LQ_QUAL_PAYLOAD_MAX  2
#define RESP_EXTRA_LQ_QUAL_MAX          RESP_EXTRA_FIELD_PRE_MAX + \
                                        RESP_EXTRA_LQ_QUAL_PAYLOAD_MAX
#define RESP_EXTRA_LQ_DOPP_PAYLOAD_MAX  4
#define RESP_EXTRA_LQ_DOPP_MAX          RESP_EXTRA_FIELD_PRE_MAX + \
                                        RESP_EXTRA_LQ_DOPP_PAYLOAD_MAX
#define RESP_EXTRA_LQ_MAX               RESP_EXTRA_LQ_QUAL_MAX + \
                                        RESP_EXTRA_LQ_DOPP_MAX
#define RESP_EXTRA_TIME_PAYLOAD_MAX     14
#define RESP_EXTRA_TIME_MAX             RESP_EXTRA_FIELD_PRE_MAX + \
                                        RESP_EXTRA_TIME_PAYLOAD_MAX

#define RESP_ALL_EXTRA_MAX              RESP_EXTRA_LQ_MAX + RESP_EXTRA_TIME_MAX

// Query Status
#define QUERY_STATUS_RESP_FIELD_PRE_MAX       1
#define QUERY_STATUS_RESP_ADDR_MAX            3
#define QUERY_STATUS_RESP_HDR_MAX             QUERY_STATUS_RESP_ADDR_MAX
#define QUERY_STATUS_RESP_VOLT_PAYLOAD_MAX    5
#define QUERY_STATUS_RESP_VOLT_MAX            QUERY_STATUS_RESP_FIELD_PRE_MAX + \
                                              QUERY_STATUS_RESP_VOLT_PAYLOAD_MAX
#define QUERY_STATUS_RESP_REL_PAYLOAD_MAX     11
#define QUERY_STATUS_RESP_REL_MAX             QUERY_STATUS_RESP_FIELD_PRE_MAX + \
                                              QUERY_STATUS_RESP_REL_PAYLOAD_MAX
#define QUERY_STATUS_RESP_BT_YEAR_PAYLOAD_MAX 10
#define QUERY_STATUS_RESP_BT_YEAR_MAX         QUERY_STATUS_RESP_FIELD_PRE_MAX + \
                                              QUERY_STATUS_RESP_BT_YEAR_PAYLOAD_MAX
#define QUERY_STATUS_RESP_BT_TIME_PAYLOAD_MAX 9
#define QUERY_STATUS_RESP_BT_TIME_MAX         QUERY_STATUS_RESP_FIELD_PRE_MAX + \
                                              QUERY_STATUS_RESP_BT_YEAR_PAYLOAD_MAX
#define QUERY_STATUS_RESP_BT_MAX              QUERY_STATUS_RESP_BT_YEAR_MAX + \
                                              QUERY_STATUS_RESP_BT_TIME_MAX
#define QUERY_STATUS_RESP_MAX                 QUERY_STATUS_RESP_HDR_MAX + \
                                              QUERY_STATUS_RESP_VOLT_MAX + \
                                              QUERY_STATUS_RESP_REL_MAX  + \
                                              QUERY_STATUS_RESP_BT_MAX

// Set Address
#define SET_ADDRESS_RESP_ADDR_MAX     3
#define SET_ADDRESS_RESP_MAX          SET_ADDRESS_RESP_ADDR_MAX

// Broadcast Message
#define BROADCAST_RESP_ADDR_MAX       3
#define BROADCAST_RESP_DATA_SIZE_MAX  2
#define BROADCAST_RESP_HDR_MAX        BROADCAST_RESP_ADDR_MAX + \
                                      BROADCAST_RESP_DATA_SIZE_MAX
#define BROADCAST_RESP_DATA_MIN       MODEM_DATA_MIN
#define BROADCAST_RESP_PAYLOAD_MAX    MODEM_PAYLOAD_MAX
#define BROADCAST_RESP_MIN            BROADCAST_RESP_HDR_MAX + \
                                      BROADCAST_RESP_DATA_MIN
#define BROADCAST_RESP_MAX            BROADCAST_RESP_HDR_MAX + \
                                      BROADCAST_RESP_PAYLOAD_MAX

// Channel Impulse
#define CHN_IMP_RESP_FIELD_PRE_MAX        1
#define CHN_IMP_RESP_MAG_COMP_MAX         1
#define CHN_IMP_RESP_ADDR_MAX             3
#define CHN_IMP_RESP_MAX                  CHN_IMP_RESP_MAG_COMP_MAX + \
                                          CHN_IMP_RESP_ADDR_MAX
#define CHN_IMP_RESP_RANGE_PAYLOAD_MAX    5
#define CHN_IMP_RESP_RANGE_MAX            CHN_IMP_RESP_FIELD_PRE_MAX + \
                                          CHN_IMP_RESP_RANGE_PAYLOAD_MAX
#define CHN_IMP_RESP_SAMP_AMT_PAYLOAD_MAX 5
#define CHN_IMP_RESP_SAMP_AMT_MAX         CHN_IMP_RESP_FIELD_PRE_MAX + \
                                          CHN_IMP_RESP_SAMP_AMT_PAYLOAD_MAX
#define CHN_IMP_RESP_MAG_SAMPLE_MAX       sizeof(uint16_t)
#define CHN_IMP_RESP_COMP_SAMPLE_MAX      sizeof(int16_t)
//TODO finish these #defines

// Noise Measurement
#define NOISE_MSR_RESP_FIELD_PRE_MAX    1
#define NOISE_MSR_RESP_RMS_PAYLOAD_MAX  6
#define NOISE_MSR_RESP_RMS_MAX          NOISE_MSR_RESP_FIELD_PRE_MAX + \
                                        NOISE_MSR_RESP_RMS_PAYLOAD_MAX
#define NOISE_MSR_RESP_P2P_PAYLOAD_MAX  6
#define NOISE_MSR_RESP_P2P_MAX          NOISE_MSR_RESP_FIELD_PRE_MAX + \
                                        NOISE_MSR_RESP_P2P_PAYLOAD_MAX
#define NOISE_MSR_RESP_MAG_PAYLOAD_MAX  6
#define NOISE_MSR_RESP_MAG_MAX          NOISE_MSR_RESP_FIELD_PRE_MAX + \
                                        NOISE_MSR_RESP_MAG_PAYLOAD_MAX
#define NOISE_MSR_RESP_MAX              NOISE_MSR_RESP_RMS_MAX + \
                                        NOISE_MSR_RESP_P2P_MAX + \
                                        NOISE_MSR_RESP_MAG_MAX                                    

// Range Data
#define RANGE_RESP_FIELD_PRE_MAX  1
#define RANGE_RESP_ADDR_MAX       3
#define RANGE_RESP_HDR_MAX        RANGE_RESP_ADDR_MAX
#define RANGE_RESP_PAYLOAD_MAX    5
#define RANGE_RESP_RANGE_MAX      RANGE_RESP_FIELD_PRE_MAX + \
                                  RANGE_RESP_RANGE_PAYLOAD_MAX
#define RANGE_RESP_MAX            RANGE_RESP_HDR_MAX + \
                                  RANGE_RESP_RANGE_MAX

// Spectrum Measurement
#define SPEC_MSR_RESP_NUM_BINS_PAYLOAD_MAX 5
#define SPEC_MSR_RESP_MAG_PAYLOAD_MAX      sizeof(unint16_t)
//TODO finish this field

// Unicast
#define UNICAST_RESP_DATA_SIZE_MAX 2
#define UNICAST_RESP_HDR_MAX       UNICAST_RESP_HDR_MAX
#define UNICAST_RESP_DATA_MIN      MODEM_DATA_MIN
#define UNICAST_RESP_PAYLOAD_MAX   MODEM_PAYLOAD_MAX
#define UNICAST_RESP_MIN           UNICAST_RESP_HDR_MAX + \
                                   UNICAST_RESP_DATA_MIN
#define UNICAST_RESP_MAX           UNICAT_RESP_HDR_MAX + \
                                   UNICAST_RESP_PAYLOAD_MAX

// Voltage and Noise Measurement (Broadcast Message)
#define VOLT_NOISE_MSR_RESP_FIELD_PRE_MAX     1
#define VOLT_NOISE_MSR_RESP_ADDR_MAX          3
#define VOLT_NOISE_MSR_RESP_DATA_SIZE_MAX     2
#define VOLT_NOISE_MSR_RESP_HDR_MAX           VOLT_NOISE_MSR_RESP_ADDR_MAX + \
                                              VOLT_NOISE_MSR_RESP_DATA_SIZE_MAX
#define VOLT_NOISE_MSR_RESP_VOLT_PAYLOAD_MAX  5
#define VOLT_NOISE_MSR_RESP_VOLT_MAX          VOLT_NOISE_MSR_RESP_FIELD_PRE_MAX + \
                                              VOLT_NOISE_MSR_RESP_VOLT_PAYLOAD_MAX
#define VOLT_NOISE_MSR_NOISE_MAX              NOISE_MSR_RESP_MAX

// Extenion Commands
#define EXTEN_RESP_TYPE_MAX         1
#define EXTEN_RESP_MAX             	EXTEN_RESP_TYPE_MAX

// --- Extension Commands ---
#define SYS_TIME_EXT_RESP_STATUS_MAX  1
#define SYS_TIME_EXT_RESP_PAYLOAD_MAX 14
#define SYS_TIME_EXT_RESP_MAX         SYS_TIME_EXT_RESP_STATUS_MAX + \
                                      SYS_TIME_EXT_RESP_PAYLOAD_MAX
#define LINK_QUAL_EXT_RESP_STATUS_MAX 1
#define LINK_QUAL_EXT_RESP_MAX        LINK_QUAL_EXT_RESP_STATUS_MAX

// --- Prefixes ---
#define ERROR_PRE                         'E'
#define MODEM_COMMAND_PRE                 '$'
#define MODEM_LOCAL_RESP_PRE              '$'
#define MODEM_RESP_PRE                    '#'
#define QUERY_STATUS_RESP_FIELD_VOLT_PRE  'V'
#define QUERY_STATUS_RESP_FIELD_REL_PRE   'R'
#define QUERY_STATUS_RESP_FIELD_BT_PRE    'B'
#define CHN_IMP_RESP_FIELD_RANGE_PRE      'T'
#define CHN_IMP_RESP_FIELD_SAMP_PRE       'L'
#define NOISE_MSR_RESP_FIELD_RMS_PRE      'R'
#define NOISE_MSR_RESP_FIELD_P2P_PRE      'P'
#define NOISE_MSR_RESP_FIELD_MAG_PRE      'M'
#define RANGE_RESP_FIELD_RANGE_PRE        'T'
#define VOLT_NOISE_MSR_FIELD_VOLT_PRE     'V'
#define RESP_EXTRA_FIELD_LQ_QUAL_PRE      'Q'
#define RESP_EXTRA_FIELD_LQ_DOPP_PRE      'D'
#define RESP_EXTRA_FIELD_TIME_PRE         'T'

// --- Postfixes ---
#define MODEM_POSTFIX '/r/n'

//Byte-align the structs. Pretty please. 
#pragma pack(push, 1)
/**
 * ------------------------------------
 *             PACKET TYPES
 * ------------------------------------
 */
// --- Modem Packet Types --
enum ModemPacketTypes_e: uint8_t {
  MODEM_COMMAND_TYPE        = '$',
  MODEM_LOCAL_RESPONSE_TYPE = '$', 
  MODEM_RESPONSE_TYPE       = '#',
  ERROR_TYPE                = 'E',
};

// --- Command Types ---
enum ModemCommandTypes_e: uint8_t {
  QUERY_STATUS_CMD_TYPE   = '?',
  SET_ADDRESS_CMD_TYPE    = 'A',
  BROADCAST_CMD_TYPE      = 'B',
  CHN_IMP_CMD_TYPE        = 'C',
  ECHO_MSG_CMD_TYPE       = 'E',
  HELP_CMD_TYPE           = 'H',
  UNICAST_ACK_CMD_TYPE    = 'M',
  NOISE_MSR_CMD_TYPE      = 'N',
  PING_CMD_TYPE           = 'P',
  CORR_ERR_CMD_TYPE       = 'Q',
  RESET_CMD_TYPE          = 'R',
  SPEC_MSR_CMD_TYPE       = 'S',
  TEST_MSG_CMD_TYPE       = 'T',
  UNICAST_MSG_CMD_TYPE    = 'U',
  VOLT_NOISE_MSR_CMD_TYPE = 'V',
  EXTEN_CMD_TYPE          = 'X',
};

// --- Extention Types --- 
enum ModemExtentionCommandTypes_e: uint8_t {
	SYS_TIME_EXT_CMD_TYPE 	= 'T',
	LINK_QUAL_EXT_CMD_TYPE 	= 'Q',
};

// --- System Time Types ---
enum SystemTimeCommandTypes_e: uint8_t {
  SYS_TIME_ENABLE_CMD_TYPE    = 'E',
  SYS_TIME_DISABLE_CMD_TYPE   = 'D',
  SYS_TIME_CLEAR_CMD_TYPE     = 'C',
  SYS_TIME_GET_TIME_CMD_TYPE  = 'G',
};

// --- Link Quality Types --- 
enum LinkQualityCommandTypes_e: uint8_t {
  LINK_QUAL_ENABLE_CMD_TYPE  = 'E',
  LINK_QUAL_DISABLE_CMD_TYPE = 'D',
};

// --- Local Response Types ---
enum ModemLocalResponseTypes_e: uint8_t {
  BROADCAST_CMD_LOCAL_RESP_TYPE  		  = 'B',
  CHN_IMP_CMD_LOCAL_RESP_TYPE    		  = 'C',
  ECHO_MSG_CMD_LOCAL_RESP_TYPE   		  = 'E',
  UNICAST_ACK_CMD_LOCAL_RESP_TYPE 	  = 'M',
  PING_CMD_LOCAL_RESP_TYPE            = 'P',
  CORR_ERR_CMD_LOCAL_RESP_TYPE        = 'C',
  RESET_CMD_LOCAL_RESP_TYPE      		  = 'R',
  SPEC_MSR_CMD_LOCAL_RESP_TYPE   		  = 'S',
  TEST_MSG_CMD_LOCAL_RESP_TYPE        = 'T',
  UNICAST_CMD_LOCAL_RESP_TYPE    		  = 'U',
  VOLT_NOISE_MSR_CMD_LOCAL_RESP_TYPE  = 'V',
};

// --- Response Types ---
enum ModemResponseTypes_e: uint8_t {
  QUERY_STATUS_RESP_TYPE  = 'A',
  SET_ADDRESS_RESP_TYPE   = 'A',
  BROADCAST_RESP_TYPE     = 'B',
  CHN_IMP_RESP_TYPE       = 'C',
  NOISE_MSR_RESP_TYPE     = 'N',
  RANGE_RESP_TYPE         = 'R',
  SPEC_MSR_RESP_TYPE      = 'S',
  UNICAST_RESP_TYPE       = 'U',
};

// --- Extention Types --- 
enum ModemExtentionResponseTypes_e: uint8_t {
	SYS_TIME_EXT_RESP_TYPE 	= 'T',
	LINK_QUAL_EXT_RESP_TYPE = 'Q',
};

// --- System Time Types ---
enum SystemTimeResponseTypes_e: uint8_t {
  SYS_TIME_ENABLE_RESP_TYPE    = 'E',
  SYS_TIME_DISABLE_RESP_TYPE   = 'D',
  SYS_TIME_CLEAR_RESP_TYPE     = 'C',
  SYS_TIME_GET_TIME_RESP_TYPE  = 'G',
};

// --- Link Quality Types --- 
enum LinkQualityResponseTypes_e: uint8_t {
  LINK_QUAL_ENABLE_RESP_TYPE  = 'E',
  LINK_QUAL_DISABLE_RESP_TYPE = 'D',
};

/**
 * ------------------------------------
 *        PACKET STRUCTURES
 * ------------------------------------
 */
// --- Modem Command Packets ---
struct SetAddresCommandPacket_t {
  uint8_t addr[SET_ADDRESS_CMD_ADDR_MAX];
};

struct BroadcastMeassageHeader_t {
  uint8_t dataSize[BROADCAST_CMD_DATA_SIZE_MAX];
};

struct BroadcastMessageCommandPacket_t {
  BroadcastMeassageHeader_t header;
  uint8_t payload[BROADCAST_CMD_PAYLOAD_MAX];
};

struct ChannelImpulseCommandPacket_t {
  uint8_t magnitudeComplex;
  uint8_t addr[CHN_IMP_CMD_ADDR_MAX];
};

struct EchoMessageCommandHeader_t {
  uint8_t addr[ECHO_MSG_CMD_ADDR_MAX];
  uint8_t dataSize[ECHO_MSG_CMD_DATA_SIZE_MAX];
};

struct EchoMessageCommandPacket_t {
  EchoMessageCommandHeader_t header;
  uint8_t payload[ECHO_MSG_CMD_PAYLOAD_MAX];
};

struct UnicastWithAckCommandHeader_t {
  uint8_t addr[UNICAST_ACK_CMD_ADDR_MAX];
  uint8_t dataSize[UNICAST_ACK_CMD_DATA_SIZE_MAX];
};

struct UnicastWithAckCommandPacket_t {
  UnicastWithAckCommandHeader_t header;
  uint8_t payload[UNICAST_ACK_CMD_PAYLOAD_MAX];
};

struct PingCommandPacket_t {
  uint8_t addr[PING_CMD_ADDR_MAX];
};

struct TestMsgCommandPacket_t {
  uint8_t addr[TEST_MSG_CMD_ADDR_MAX];
};

struct UnicastCommandHeader_t {
  uint8_t addr[UNICAST_CMD_ADDR_MAX];
  uint8_t dataSize[UNICAST_CMD_DATA_SIZE_MAX];
};

struct UnicastCommandPacket_t {
  UnicastCommandHeader_t header;
  uint8_t payload[UNICAST_CMD_PAYLOAD_MAX];
};

struct VoltageAndNoiseCommandPacket_t {
  uint8_t addr[VOLT_NOISE_MSR_CMD_ADDR_MAX];
};

// Extenion Commands
struct SystemTimeCommandPacket_t {
  SystemTimeCommandTypes_e type;
};

struct LinkQualityCommandPacket_t {
  LinkQualityCommandTypes_e type;
};

union ExtenionCommandPacketVariant_u {
  SystemTimeCommandPacket_t systemTime;
  LinkQualityCommandPacket_t linkQuality;
};

struct ExtenionCommandPacket_t{
  ModemExtentionCommandTypes_e type;
  ExtenionCommandPacketVariant_u command;
};

union ModemCommandPacketVariant_u {
  SetAddresCommandPacket_t setAddress;
  BroadcastMessageCommandPacket_t broadcastMessage;
  ChannelImpulseCommandPacket_t channelImpulse;
  EchoMessageCommandPacket_t echoMessage;
  UnicastWithAckCommandPacket_t unicastWithAck;
  PingCommandPacket_t ping;
  TestMsgCommandPacket_t testMsg;
  UnicastCommandPacket_t unicast;
  VoltageAndNoiseCommandPacket_t voltageAndNoise;
  ExtenionCommandPacket_t extPacket;
};

struct ModemCommandPacket_t {
  ModemCommandTypes_e type; 
  ModemCommandPacketVariant_u command;
};

// --- Local Response Commands ---
struct BroadcastLocalResponsePacket_t {
  uint8_t dataSize[BROADCAST_CMD_LOCAL_RESP_DATA_SIZE_MAX];
};

struct ChannelImpulseLocalResponsePacket_t {
  uint8_t magnitudeComplex;
  uint8_t addr[CHN_IMP_CMD_LOCAL_RESP_ADDR_MAX];
};

struct EchoMessageLocalResponsePacket_t {
  uint8_t addr[ECHO_MSG_CMD_LOCAL_RESP_ADDR_MAX];
  uint8_t dataSize[ECHO_MSG_CMD_LOCAL_RESP_DATA_SIZE_MAX];
};

struct UnicastWithAckLocalResponsePacket_t{
  uint8_t addr[UNICAST_ACK_CMD_LOCAL_RESP_ADDR_MAX];
  uint8_t dataSize[UNICAST_ACK_CMD_LOCAL_RESP_DATA_SIZE_MAX];
};

struct PingLocalResponsePacket_t {
  uint8_t addr[PING_CMD_LOCAL_RESP_ADDR_MAX];
};

struct CorrectedErrorsLocalResponsePacket_t {
  uint8_t rS;
};

struct TestMessageLocalResponsePacket_t {
  uint8_t addr[TEST_MSG_CMD_LOCAL_RESP_ADDR_MAX];
};

struct UnicastLocalResponsePacket_t {
  uint8_t addr[UNICAST_CMD_LOCAL_RESP_ADDR_MAX];
  uint8_t dataSize[UNICAST_ACK_CMD_LOCAL_RESP_DATA_SIZE_MAX];
};

struct VoltageAndNoiseLocalResponsePacket_t {
  uint8_t addr[VOLT_NOISE_MSR_CMD_LOCAL_RESP_ADDR_MAX];
};

union ModemLocalResponsePacketVariant_u {
  BroadcastLocalResponsePacket_t broadcast;
  ChannelImpulseLocalResponsePacket_t channelImpulse;
  EchoMessageLocalResponsePacket_t echoMessage;
  UnicastWithAckLocalResponsePacket_t unicastWithAck;
  PingLocalResponsePacket_t ping;
  CorrectedErrorsLocalResponsePacket_t correctedErorrs;
  TestMessageLocalResponsePacket_t testMessage;
  UnicastLocalResponsePacket_t unicast;
  VoltageAndNoiseLocalResponsePacket_t voltageAndNoise;
};

struct ModemLocalResponsePacket_t {
  ModemLocalResponseTypes_e type;
  ModemLocalResponsePacketVariant_u response;
};

// --- Modem Response Packets ---
struct ResponseExtraLinkQuality_t {
  uint8_t lqQualSep;
  uint8_t lqQualPayload[RESP_EXTRA_LQ_QUAL_PAYLOAD_MAX];
  uint8_t lqDoppSep;
  uint8_t lqDoppPayload[RESP_EXTRA_LQ_DOPP_PAYLOAD_MAX];
};

struct ResponseExtraTime_t {
  uint8_t timeSep;
  uint8_t timePayload[RESP_EXTRA_TIME_PAYLOAD_MAX];
};

struct ResponseExtraAll_t{
  ResponseExtraLinkQuality_t lqData;
  ResponseExtraTime_t timeData;
};

union ResponseExtraDataVariant_u{
  ResponseExtraLinkQuality_t linkQuality;
  ResponseExtraTime_t time;
  ResponseExtraAll_t all;
};

// Response Extra Data (Link Quality and/or Time)
struct ResponseExtraData_t {
  ResponseExtraDataVariant_u data;
};

struct SetAddressResponsePacket_t {
  uint8_t addr[SET_ADDRESS_RESP_ADDR_MAX];
};

struct QueryStatusResponseBuildTime_t {
  uint8_t buildTimeYearPayload[QUERY_STATUS_RESP_BT_YEAR_PAYLOAD_MAX];
  uint8_t buildTimeTimeSep;
  uint8_t buildTimeTimePayload[QUERY_STATUS_RESP_BT_TIME_PAYLOAD_MAX];
};

struct QueryStatusResponseFullPacket_t {
  uint8_t addr[QUERY_STATUS_RESP_ADDR_MAX];
  uint8_t voltSep;
  uint8_t voltPayload[QUERY_STATUS_RESP_VOLT_PAYLOAD_MAX];
  uint8_t releaseSep;
  uint8_t releasePayload[QUERY_STATUS_RESP_REL_PAYLOAD_MAX];
  uint8_t buildTimeSep;
  QueryStatusResponseBuildTime_t buildTime;
};

union QueryStatusResponsePacketVariant_u{
  SetAddressResponsePacket_t setAddress;
  QueryStatusResponseFullPacket_t fullStatus;
};

struct QueryStatusResponsePacket_t {
  QueryStatusResponsePacketVariant_u status;
};

struct BroadcastMessageHeader_t {
  uint8_t addr[BROADCAST_RESP_ADDR_MAX];
  uint8_t dataSize[BROADCAST_RESP_DATA_SIZE_MAX];
};

struct BroadcastMessageResponsePacket_t {
  BroadcastMessageHeader_t header;
  BroadcastMessageResponseVariant_u message;
};

struct NoiseMeasurementResponsePacket_t {
  uint8_t rmsSep;
  uint8_t rmsPayload[NOISE_MSR_RESP_RMS_PAYLOAD_MAX];
  uint8_t p2pSep;
  uint8_t p2pPayload[NOISE_MSR_RESP_P2P_PAYLOAD_MAX];
  uint8_t magSep;
  uint8_t magPayload[NOISE_MSR_RESP_MAG_PAYLOAD_MAX];
};

struct VoltageAndNoiseResponsePacket_t {
  uint8_t voltSep;
  uint8_t voltPayload[VOLT_NOISE_MSR_RESP_VOLT_PAYLOAD_MAX];
  NoiseMeasurementResponsePacket_t noiseMeasurmeent;
};

union BroadcastMessageResponseVariant_u {
  uint8_t payload[BROADCAST_RESP_PAYLOAD_MAX];
  VoltageAndNoiseResponsePacket_t voltAndNoise;
};

struct RangeDataResponsePacket_t {
  uint8_t addr[RANGE_RESP_ADDR_MAX];
  uint8_t rangeSep;
  uint8_t rangePayload[RANGE_RESP_PAYLOAD_MAX];
};

struct UnicastResponsePacket_t {
  uint8_t dataSize[UNICAST_RESP_DATA_SIZE_MAX];
  uint8_t payload[UNICAST_RESP_PAYLOAD_MAX];
};

// Extention Packets
struct SystemTimeResponsePacket_t {
  uint8_t status;
  uint8_t timePayload[SYS_TIME_EXT_RESP_PAYLOAD_MAX];
};

struct LinkQualityResponsePacket_t {
  uint8_t status;
};

union ExtenionResponsePacketVariant_u {
  SystemTimeResponsePacket_t systemTime;
  LinkQualityResponsePacket_t linkQuality;
};
struct ExtenionResponsePacket_t {
  ModemExtentionResponseTypes_e type;
  ExtenionResponsePacketVariant_u response;
};

union ModemResponsePacketVariant_u {
  QueryStatusResponsePacket_t queryStatus;
  BroadcastMessageResponsePacket_t broadcast;
  RangeDataResponsePacket_t rangeData;
  UnicastResponsePacket_t unicast;
  ExtenionResponsePacket_t extPacket;
};

struct ModemResponsePacket_t {
  ModemResponseTypes_e type;
  ModemResponsePacketVariant_u response;
  ResponseExtraData_t extraData;
};

// --- Modem Packet ---
union ModemPacketVariant_u {
  ModemCommandPacket_t command;
  ModemLocalResponsePacket_t localResponse;
  ModemResponsePacket_t response;
};

struct ModemPacket_t {
  ModemPacketTypes_e type;
  ModemPacketVariant_u payload;
};

#pragma pack(pop)

void print_packet(String packetBuffer, String packet_type);
void query_status(HardwareSerial connection);
void set_address(HardwareSerial connection, int8_t addr);
uint8_t get_modem_address();
void broadcast(HardwareSerial connection, char *data, int8_t bytes);
void ping(HardwareSerial connection, int8_t addr);
void parse_status_query_packet(QueryStatusResponseFullPacket_t* statusResponse);
void parse_broadcast_packet(uint8_t* packetBuffer, uint8_t size);
void parse_unicast_packet(uint8_t* packetBuffer, uint8_t size);
void packet_received_modem(uint8_t* packetBuffer, uint8_t size);
void packet_received_nest(uint8_t* packetBuffer, uint8_t size);


#endif