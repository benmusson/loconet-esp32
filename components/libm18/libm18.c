#include <libm18.h>
#include <string.h>


void m18_reset_acc(M18_Context *context) {
    context->acc = 0x04;
}

void m18_update_acc(M18_Context *context) {
    switch (context->acc) {
        case 0x04: context->acc = 0x0C;
            break;
        case 0x0C: context->acc = 0x1C;
            break;
        case 0x1C: context->acc = 0x04;
            break;
        default: context->acc = 0x04;
    }
}

void m18_init_message_read(M18_Message *message, M18_DataId request_id) {
    if (!message) return;

    const M18_DataInfo *info = m18_get_data_info(request_id);
    if (!info) return;

    memset(message->data, 0, sizeof(message->data));
    message->size = 8;

    message->data[0] = M18_CMD_SYNC;
    message->data[1] = M18_CMD_READ;
    message->data[2] = 0x04;
    message->data[3] = 0x03;
    message->data[4] = M18_MSB(info->addr);
    message->data[5] = M18_LSB(info->addr);
    message->data[6] = info->len;
    message->data[7] = m18_checksum((uint8_t *) message, message->size);
}

void m18_init_message_calibration(M18_Context *context, M18_Message_Calibration *message) {
    message->command  = M18_CMD_CALIBRATE;
    message->acc      = context->acc;
    message->checksum = m18_checksum((uint8_t *) message, sizeof(*message));
    m18_update_acc(context);
}

void m18_init_message_configure(M18_Context *context, M18_Message_Configure *message, uint8_t state) {
    m18_reset_acc(context);
    message->command        = M18_CMD_CONFIGURE;
    message->acc            = context->acc;
    message->magic1         = 8; // magic number
    message->cutoff_current = M18_CUTOFF_CURRENT;
    message->max_current1   = M18_MAX_CURRENT;
    message->max_current1   = M18_MAX_CURRENT;
    message->state          = state;
    message->magic2         = 13; // magic number
    message->checksum       = m18_checksum((uint8_t *) message, sizeof(*message));
}

void m18_init_message_keepalive(const M18_Context *context, M18_Message_KeepAlive *message) {
    message->command  = M18_CMD_KEEPALIVE;
    message->acc      = context->acc;
    message->checksum = m18_checksum((uint8_t *) message, sizeof(*message));
}

void m18_init_message_snapchat(M18_Context *context, M18_Message_Snapchat *message) {
    message->command  = M18_CMD_SNAPCHAT;
    message->acc      = context->acc;
    message->checksum = m18_checksum((uint8_t *) message, sizeof(*message));
    m18_update_acc(context);
}

void m18_init_message_sync(M18_Context *context, M18_Message_Sync *message) {
    message->command  = M18_CMD_SYNC;
    message->checksum = m18_checksum((uint8_t *) message, sizeof(*message));
}


