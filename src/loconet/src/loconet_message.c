#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "loconet.h"
#include "loconet_message.h"

LocoNetMessageRaw loconet_message_raw_start(void) {
    LocoNetMessageRaw raw = {
        .data = {0},
        .size = 0,
        .checksum = 0xFF
    };
    return raw;
}

LocoNetIOBufferStatus loconet_message_raw_add(LocoNetMessageRaw *unfinished, uint8_t new_byte) {

    if (unfinished->size < LOCONET_IO_BUFFER_SIZE)
    {
        unfinished->data[unfinished->size] = new_byte;
        unfinished->checksum ^= new_byte;
        unfinished->size++;
        return LOCONET_BUFFER_GOOD;
    } else
    {
        return LOCONET_BUFFER_FULL;
    }
}

LocoNetMessageRaw loconet_message_raw_end(LocoNetMessageRaw *unfinished) {
    return *unfinished;
}

LocoNetChecksumStatus loconet_message_raw_verify_checksum(const LocoNetMessageRaw *unfinished) {
    if (unfinished->checksum == 0x00) {
        return LOCONET_CHECKSUM_GOOD;
    }
    else {
        return LOCONET_CHECKSUM_BAD;
    }
}

char *loconet_opcode_to_string(uint8_t opcode) {
    switch (opcode) {
        case (LOCONET_OPC_SW_REQ): {
            return "Switch Request";
            break;
        }
        case (LOCONET_OPC_SW_STATE): {
            return "Switch Request State";
            break;
        }
        default: {
            return "Unknown";
        }
    };
}

LocoNetMessageLongAck loconet_message_long_ack_deserialize(const LocoNetMessageRaw *raw) {
    return (LocoNetMessageLongAck) {
        .opcode = raw->data[1] | 0b10000000,
        .response = raw->data[2]
    };
}

LocoNetMessageRaw loconet_message_long_ack_serialize(const LocoNetMessageLongAck *message) {
    LocoNetMessageRaw raw = loconet_message_raw_start();

    loconet_message_raw_add(&raw, LOCONET_OPC_LONG_ACK);
    loconet_message_raw_add(&raw, message->opcode);
    loconet_message_raw_add(&raw, message->response);
    loconet_message_raw_add(&raw, raw.checksum);
    loconet_message_raw_end(&raw);

    return raw;
}

LocoNetMessageLocoSpeed loconet_message_loco_speed_deserialize(const LocoNetMessageRaw *raw) {
    return (LocoNetMessageLocoSpeed) {
        .slot = raw->data[1],
        .speed = raw->data[2]
    };
}

LocoNetMessageRaw loconet_message_loco_speed_serialize(const LocoNetMessageLocoSpeed *message) {
    LocoNetMessageRaw raw = loconet_message_raw_start();

    loconet_message_raw_add(&raw, LOCONET_OPC_LOCO_SPD);
    loconet_message_raw_add(&raw, message->slot);
    loconet_message_raw_add(&raw, message->speed);
    loconet_message_raw_add(&raw, raw.checksum);
    loconet_message_raw_end(&raw);

    return raw;
}

LocoNetMessageSwitchRequest loconet_message_switch_request_deserialize(const LocoNetMessageRaw *raw) {
    return (LocoNetMessageSwitchRequest) {
        .address = (raw->data[1] & LOCONET_MASK_SW_ADDRESS_LSB) + ((raw->data[2] & LOCONET_MASK_SW_ADDRESS_MSB) << 7) + 1,
        .output = (raw->data[2] & LOCONET_MASK_SW_OUTPUT) ? LOCONET_SWITCH_OUTPUT_ON : LOCONET_SWITCH_OUTPUT_OFF,
        .direction = (raw->data[2] & LOCONET_MASK_SW_DIRECTION) ? LOCONET_SWITCH_OUTPUT_ON : LOCONET_SWITCH_OUTPUT_OFF,
    };
}

LocoNetMessageRaw loconet_message_switch_request_serialize(const LocoNetMessageSwitchRequest *message) {
    LocoNetMessageRaw raw = loconet_message_raw_start();

    loconet_message_raw_add(&raw, LOCONET_OPC_SW_REQ);
    loconet_message_raw_add(&raw, (message->address - 1) & LOCONET_MASK_SW_ADDRESS_LSB);

    const uint8_t output = message->output ? LOCONET_MASK_SW_OUTPUT : 0;
    const uint8_t direction = message->direction ? LOCONET_MASK_SW_DIRECTION : 0;
    loconet_message_raw_add(&raw, (((message->address- 1) >> 7) & LOCONET_MASK_SW_ADDRESS_MSB) | output | direction);
    loconet_message_raw_add(&raw, raw.checksum);
    loconet_message_raw_end(&raw);

    return raw;
}

LocoNetMessageSwitchStateRequest loconet_message_switch_state_request_deserialize(const LocoNetMessageRaw *raw) {
    return (LocoNetMessageSwitchStateRequest) {
        .address = (raw->data[1] & LOCONET_MASK_SW_ADDRESS_LSB) + ((raw->data[2] & LOCONET_MASK_SW_ADDRESS_MSB) << 7) + 1,
    };
}
LocoNetMessageRaw loconet_message_switch_state_request_serialize(const LocoNetMessageSwitchStateRequest *message) {
    LocoNetMessageRaw raw = loconet_message_raw_start();

    loconet_message_raw_add(&raw, LOCONET_OPC_SW_REQ);
    loconet_message_raw_add(&raw, (message->address - 1) & LOCONET_MASK_SW_ADDRESS_LSB);
    loconet_message_raw_add(&raw, ((message->address- 1) >> 7) & LOCONET_MASK_SW_ADDRESS_MSB);
    loconet_message_raw_add(&raw, raw.checksum);
    loconet_message_raw_end(&raw);

    return raw;

}