#ifndef LOCONET_H
#define LOCONET_H

#include <stdint.h>

#include "loconet_message.h"


#define LOCONET_BAUD_RATE 16667
#define LOCONET_MESSAGE_BUFFER_SIZE 128

typedef struct LocoNetInterface LocoNetInterface;
typedef struct LocoNetInterface *LocoNetInterfaceHandle;
typedef void (*LocoNetMessageCallback)(LocoNetInterfaceHandle handle, LocoNetMessageRaw *message);


LocoNetInterfaceHandle loconet_interface_new(void);
void loconet_interface_delete(LocoNetInterfaceHandle handle);
void loconet_interface_register_cb_read(LocoNetInterfaceHandle handle, const LocoNetMessageCallback cb);
void loconet_interface_register_cb_write(LocoNetInterfaceHandle handle, const LocoNetMessageCallback cb);


uint8_t loconet_message_receive(LocoNetInterfaceHandle *handle, LocoNetMessageRaw *message);
uint8_t loconet_message_send(LocoNetInterfaceHandle *handle, LocoNetMessageRaw *message);


#endif /* !LOCONET_H */