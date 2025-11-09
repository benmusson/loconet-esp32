#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "hal/uart_hal.h"
#include "esp_log.h"

#include "loconet_esp32.h"

#define LOCONET_BUFFER_SIZE (1024)
#define LOCONET_READ_BUFFER_SIZE (LOCONET_BUFFER_SIZE)
#define LOCONET_UART_BUFFER_SIZE (LOCONET_BUFFER_SIZE * 2)
#define LOCONET_UART_TASK_SIZE (1024 * 10)
#define LOCONET_UART_QUEUE_SIZE 20

static const char *TAG = "loconet_uart_events";
void loconet_uart_message_send(LocoNetUartInterfaceHandle handle, LocoNetMessageRaw *message);

typedef struct LocoNetUartInterface {
    LocoNetMessageCallback read_callback;
    LocoNetMessageCallback write_callback;
    LocoNetMessageSendImpl send;
    LocoNetUartConfig uart_config;
    QueueHandle_t uart_queue;
    TaskHandle_t task_handle;
} LocoNetUartInterface ;

static uart_config_t loconet_esp32_uart_config = {
    .baud_rate = LOCONET_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};

LocoNetUartInterfaceHandle loconet_uart_handle_new(LocoNetUartConfig *uart_config) {
    LocoNetUartInterfaceHandle handle = malloc(sizeof(LocoNetUartInterface));

    (*handle).uart_config = *uart_config;
    (*handle).uart_queue = NULL;
    (*handle).send = (LocoNetMessageSendImpl)loconet_uart_message_send;

    return handle;
}

LocoNetUartConfig *loconet_uart_config_get(LocoNetUartInterfaceHandle handle) {
    return &(handle->uart_config);
}

QueueHandle_t *loconet_uart_queue_get(LocoNetUartInterfaceHandle handle) {
    return &(handle->uart_queue);
}

TaskHandle_t *loconet_uart_task_handle_get(LocoNetUartInterfaceHandle handle) {
    return &(handle->task_handle);
}

static void loconet_uart_event_task(void *pvParameters);
LocoNetUartInterfaceHandle loconet_uart_start(LocoNetUartConfig uart_config) {

    ESP_LOGI(TAG, "Starting LocoNet interface on UART%d", uart_config.uart_port);

    // Setup UART buffered IO with event queue
    LocoNetUartInterfaceHandle handle = loconet_uart_handle_new(&uart_config);
    QueueHandle_t *queue = loconet_uart_queue_get(handle);
    TaskHandle_t *task_handle = loconet_uart_task_handle_get(handle);

    // Install UART driver using an event queue
    ESP_ERROR_CHECK(uart_driver_install(uart_config.uart_port, LOCONET_UART_BUFFER_SIZE, LOCONET_UART_BUFFER_SIZE, LOCONET_UART_QUEUE_SIZE, queue, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_config.uart_port, &loconet_esp32_uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_config.uart_port, uart_config.tx_pin, uart_config.rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    if (uart_config.tx_invert) ESP_ERROR_CHECK(uart_set_line_inverse(uart_config.uart_port, UART_SIGNAL_TXD_INV));
    if (uart_config.rx_invert) ESP_ERROR_CHECK(uart_set_line_inverse(uart_config.uart_port, UART_SIGNAL_RXD_INV));

    // Set the UART driver for manual flow control
    ESP_ERROR_CHECK(uart_set_mode(uart_config.uart_port, UART_MODE_RS485_HALF_DUPLEX));

    char task_name[20];
    snprintf(task_name, 20, "loconet_uart_%d", uart_config.uart_port);
    xTaskCreate(loconet_uart_event_task, task_name, LOCONET_UART_TASK_SIZE, &handle, 12, task_handle);

    return handle;
}

void loconet_uart_stop(LocoNetUartInterfaceHandle handle) {
    ESP_LOGI(TAG, "Stopping LocoNet event task for UART%d", loconet_uart_config_get(handle)->uart_port);
    vTaskDelete(*loconet_uart_task_handle_get(handle));
    uart_driver_delete(loconet_uart_config_get(handle)->uart_port);
}

void loconet_uart_message_send(LocoNetUartInterfaceHandle handle, LocoNetMessageRaw *message) {
    uart_write_bytes(handle->uart_config.uart_port, message->data, message->size);

    if (handle->write_callback) {
        handle->write_callback((LocoNetInterfaceHandle)handle, message);
    }
}

void loconet_uart_message_received(LocoNetUartInterfaceHandle handle, LocoNetMessageRaw *message) {
    if (loconet_message_raw_verify_checksum(message) == LOCONET_CHECKSUM_BAD) {
        ESP_LOGW(TAG, "Received a message that failed checksum validation:");
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, message->data, message->size, ESP_LOG_WARN);
        return;
    }

    if (handle->read_callback) {
        handle->read_callback((LocoNetInterfaceHandle)handle, message);
    }
}

static void loconet_uart_event_task(void *pvParameters) {
    LocoNetUartInterfaceHandle handle = *(LocoNetUartInterfaceHandle *)pvParameters;

    LocoNetUartConfig uart_config = *loconet_uart_config_get(handle);
    const QueueHandle_t queue = *loconet_uart_queue_get(handle);
    const uart_port_t uart_port = uart_config.uart_port;

    uart_event_t event;
    uint8_t* read_buffer = malloc(LOCONET_READ_BUFFER_SIZE);
    LocoNetMessageRaw raw = loconet_message_raw_start();
    ESP_LOGI(TAG, "Starting LocoNet event task for UART%d", uart_port);

    for (;;) {
        if (xQueueReceive(queue, (void *)&event, (TickType_t)portMAX_DELAY)) {

            bzero(read_buffer, LOCONET_READ_BUFFER_SIZE);
            ESP_LOGD(TAG, "UART%d event:", uart_port);
            
            switch (event.type) {

            case UART_DATA:
                ESP_LOGD(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(uart_port, read_buffer, event.size, portMAX_DELAY);
                ESP_LOG_BUFFER_HEX_LEVEL(TAG, read_buffer, event.size, ESP_LOG_VERBOSE);

                // Start a new message, and append bytes until either the buffer is full, or the beginning of a new message is found.
                raw = loconet_message_raw_start();
                for (size_t i = 0; i < event.size; ++i) {

                    const uint8_t newByte = read_buffer[i];
                    
                    // Back-to-back messages detected.
                    if( (newByte & LOCONET_MASK_ANY_OPCODE) && (i > 2)) {
                        // End the current message, and start a new one.
                        loconet_message_raw_end(&raw);
                        loconet_uart_message_received(handle, &raw);
                        raw = loconet_message_raw_start();
                    } 

                    loconet_message_raw_add(&raw, newByte);
                }

                loconet_message_raw_end(&raw);
                loconet_uart_message_received(handle, &raw);
                break;

            case UART_FIFO_OVF:
                ESP_LOGW(TAG, "hw fifo overflow");
                // Directly flush the rx buffer in order to read more data.
                uart_flush_input(uart_port);
                xQueueReset(queue);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGW(TAG, "ring buffer full");
                // Directly flush the rx buffer in order to read more data.
                uart_flush_input(uart_port);
                xQueueReset(queue);
                break;

            case UART_BREAK:
                ESP_LOGV(TAG, "uart rx break");
                break;

            case UART_PARITY_ERR:
                ESP_LOGW(TAG, "uart parity error");
                break;

            case UART_FRAME_ERR:
                ESP_LOGW(TAG, "uart frame error");
                break;

            default:
                ESP_LOGV(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }

    free(read_buffer);
    read_buffer = NULL;

    vTaskDelete(handle->task_handle);
    handle->task_handle = NULL; 
}