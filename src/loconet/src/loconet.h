#ifndef LOCONET_H
#define LOCONET_H

#include <stdint.h>

#include "loconet_message.h"


#define LOCONET_BAUD_RATE 16667

typedef struct LocoNetInterface LocoNetInterface;
typedef struct LocoNetInterface *LocoNetInterfaceHandle;
typedef void (*LocoNetMessageCallback)(LocoNetInterfaceHandle handle, LocoNetMessageRaw *message);
typedef void (*LocoNetMessageSendImpl)(LocoNetInterfaceHandle handle, LocoNetMessageRaw *message);


LocoNetInterfaceHandle loconet_interface_new(void);
void loconet_interface_delete(LocoNetInterfaceHandle handle);
void loconet_message_send(LocoNetInterfaceHandle handle, LocoNetMessageRaw *message);
void loconet_interface_register_cb_read(LocoNetInterfaceHandle handle, const LocoNetMessageCallback cb);
void loconet_interface_register_cb_write(LocoNetInterfaceHandle handle, const LocoNetMessageCallback cb);



#endif /* !LOCONET_H */