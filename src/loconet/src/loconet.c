#include <stdlib.h>

#include "loconet.h"

typedef struct LocoNetInterface {
    LocoNetMessageCallback read_callback;
    LocoNetMessageCallback write_callback;
} LocoNetInterface ;

LocoNetInterfaceHandle loconet_interface_new(void) {
    LocoNetInterfaceHandle handle = malloc(sizeof(LocoNetInterface));
    handle->read_callback = NULL;
    handle->write_callback = NULL;
    return handle;
}

void loconet_interface_delete(LocoNetInterfaceHandle handle) {
    free(handle);
}

void loconet_interface_register_cb_read(LocoNetInterfaceHandle handle, const LocoNetMessageCallback cb) {
    handle->read_callback = cb;
}

void loconet_interface_register_cb_write(LocoNetInterfaceHandle handle, const LocoNetMessageCallback cb) {
    handle->write_callback = cb;
}

