/* Borrowed from JMRI LnConstants class.*/

#ifndef LOCONET_MESSAGE_H
#define LOCONET_MESSAGE_H

#define LOCONET_IO_BUFFER_SIZE 16

#include <stdint.h>

#include "loconet.h"

/* OPCode Definitions */
typedef enum LocoNetOPCode {
    LOCONET_OPC_GPBUSY = 0x81,
    LOCONET_OPC_GPOFF = 0x82,
    LOCONET_OPC_GPON = 0x83,
    LOCONET_OPC_IDLE = 0x85,
    LOCONET_OPC_RE_LOCORESET_BUTTON = 0x8A,
    LOCONET_OPC_LOCO_SPD = 0xa0,
    LOCONET_OPC_LOCO_DIRF = 0xa1,
    LOCONET_OPC_LOCO_SND = 0xa2,
    LOCONET_OPC_SW_REQ = 0xb0,
    LOCONET_OPC_SW_REP = 0xb1,
    LOCONET_OPC_INPUT_REP = 0xb2,
    LOCONET_OPC_UNKNOWN = 0xb3,
    LOCONET_OPC_LONG_ACK = 0xb4,
    LOCONET_OPC_SLOT_STAT1 = 0xb5,
    LOCONET_OPC_CONSIST_FUNC = 0xb6,
    LOCONET_OPC_UNLINK_SLOTS = 0xb8,
    LOCONET_OPC_LINK_SLOTS = 0xb9,
    LOCONET_OPC_MOVE_SLOTS = 0xba,
    LOCONET_OPC_RQ_SL_DATA = 0xbb,
    LOCONET_OPC_SW_STATE = 0xbc,
    LOCONET_OPC_SW_ACK = 0xbd,
    LOCONET_OPC_LOCO_ADR = 0xbf,
    LOCONET_OPC_MULTI_SENSE = 0xd0, 
    LOCONET_OPC_MULTI_SENSE_LONG = 0xe0,
    LOCONET_OPC_PANEL_RESPONSE = 0xd7,
    LOCONET_OPC_PANEL_QUERY = 0xdf,
    LOCONET_OPC_LISSY_UPDATE = 0xe4,
    LOCONET_OPC_PEER_XFER = 0xe5,
    LOCONET_OPC_ALM_READ = 0xe6,
    LOCONET_OPC_SL_RD_DATA = 0xe7,
    LOCONET_OPC_IMM_PACKET = 0xed,
    LOCONET_OPC_IMM_PACKET_2 = 0xee,
    LOCONET_OPC_WR_SL_DATA = 0xef,
    LOCONET_OPC_ALM_WRITE = 0xee,
    LOCONET_OPC_MASK = 0x7f,
    LOCONET_OPC_EXP_REQ_SLOT = 0xbe,
    LOCONET_OPC_EXP_SLOT_MOVE = 0xd4,
    LOCONET_OPC_EXP_RD_SL_DATA = 0xe6,
    LOCONET_OPC_EXP_WR_SL_DATA = 0xee,
    LOCONET_OPC_MAX
} LocoNetOPCode ;

/* Bit Masks */
#define LOCONET_MASK_ANY_OPCODE     0x10000000 // Any LocoNet OPCode
#define LOCONET_MASK_SW_ADDRESS_MSB 0x00001111 // Switch Address Mask, most significant byte
#define LOCONET_MASK_SW_ADDRESS_LSB 0x01111111 // Switch Address Mask, least significant byte
#define LOCONET_MASK_SW_DIRECTION   0x00100000 // Switch Direction Mask
#define LOCONET_MASK_SW_OUTPUT      0x00010000 // Switch Output Mask 

// Long Acknowledge Message
typedef struct LocoNetMessageLongAck {
    uint8_t opcode;
    uint8_t response;
} LocoNetMessageLongAck ;

// Set Loco Speed Message
typedef struct LocoNetMessageLocoSpeed {
    uint8_t slot;
    uint8_t speed; 
} LocoNetMessageLocoSpeed ;

typedef enum LocoNetSwitchDirection {
    LOCONET_SWITCH_DIRECTION_OPEN = 1,
    LOCONET_SWITCH_DIRECTION_CLOSED = 0
} LocoNetSwitchDirection ;

typedef enum LocoNetSwitchOutput {
    LOCONET_SWITCH_OUTPUT_ON = 1,
    LOCONET_SWITCH_OUTPUT_OFF = 0
} LocoNetSwitchOutput ;

// Request Switch (write switch state)
typedef struct LocoNetMessageSwitchRequest {
    uint16_t address;
    LocoNetSwitchDirection direction;
    LocoNetSwitchOutput output;
} LocoNetMessageSwitchRequest ;

// Request Switch State (read switch state)
typedef struct LocoNetMessageSwitchStateRequest {
    uint16_t address;
} LocoNetMessageSwitchStateRequest ;



typedef struct LocoNetMessageRaw {
    union {
        uint8_t opcode;
        uint8_t data[LOCONET_IO_BUFFER_SIZE];
    };
    uint8_t checksum;
    uint8_t size;
} LocoNetMessageRaw ;

typedef enum LocoNetIOBufferStatus {
    LOCONET_BUFFER_GOOD,
    LOCONET_BUFFER_FULL,    
} LocoNetIOBufferStatus ;

typedef enum LocoNetChecksumStatus
{
    LOCONET_CHECKSUM_GOOD,
    LOCONET_CHECKSUM_BAD,
} LocoNetChecksumStatus ;

LocoNetMessageRaw loconet_message_raw_start(void);
LocoNetIOBufferStatus loconet_message_raw_add(LocoNetMessageRaw *unfinished, const uint8_t new_byte);
LocoNetChecksumStatus loconet_message_raw_verify_checksum(const LocoNetMessageRaw *unfinished);
LocoNetMessageRaw loconet_message_raw_end(LocoNetMessageRaw *unfinished);

char *loconet_opcode_to_string(uint8_t opcode);

LocoNetMessageLongAck loconet_message_long_ack_deserialize(const LocoNetMessageRaw *raw);
LocoNetMessageRaw loconet_message_long_ack_serialize(const LocoNetMessageLongAck *message);

LocoNetMessageLocoSpeed loconet_message_loco_speed_deserialize(const LocoNetMessageRaw *raw);
LocoNetMessageRaw loconet_message_loco_speed_serialize(const LocoNetMessageLocoSpeed *message);

LocoNetMessageSwitchRequest loconet_message_switch_request_deserialize(const LocoNetMessageRaw *raw);
LocoNetMessageRaw loconet_message_switch_request_serialize(const LocoNetMessageSwitchRequest *message);

LocoNetMessageSwitchStateRequest loconet_message_switch_state_request_deserialize(const LocoNetMessageRaw *raw);
LocoNetMessageRaw loconet_message_switch_state_request_serialize(const LocoNetMessageSwitchStateRequest *message);

#endif /* !LOCONET_MESSAGE_H */