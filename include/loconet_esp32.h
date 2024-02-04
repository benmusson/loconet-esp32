#ifndef LOCONET_ESP32_H
#define LOCONET_ESP32_H

#include "driver/uart.h"

#include "loconet.h"

typedef struct LocoNetUartConfig {
    uart_port_t uart_port;
    uint8_t tx_pin;
    uint8_t rx_pin;
    bool tx_invert;
    bool rx_invert;
} LocoNetUartConfig ;

typedef struct LocoNetUartInterface LocoNetUartInterface;
typedef struct LocoNetUartInterface *LocoNetUartInterfaceHandle;
typedef void (*LocoNetUartMessageCallback)(LocoNetUartInterfaceHandle handle, LocoNetMessageRaw *message);

LocoNetUartInterfaceHandle loconet_uart_handle_new(LocoNetUartConfig *uart_config);
QueueHandle_t *loconet_uart_queue_get(LocoNetUartInterfaceHandle handle);
LocoNetUartConfig *loconet_uart_config_get(LocoNetUartInterfaceHandle handle);
void loconet_uart_message_send(LocoNetUartInterfaceHandle handle, LocoNetMessageRaw *message);

LocoNetUartInterfaceHandle loconet_uart_start(LocoNetUartConfig uart_config);
void loconet_uart_stop(LocoNetUartInterfaceHandle handle);

#endif /* !LOCONET_ESP32_H */