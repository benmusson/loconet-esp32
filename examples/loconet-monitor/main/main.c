#include "esp_log.h"

#include "loconet.h"
#include "loconet_esp32.h"

#define QUEUE_SIZE 16

static const char *TAG = "loconet-monitor";

LocoNetUartConfig loconet_uart_config = {
    .uart_port = 1,
    .tx_pin = 5,
    .rx_pin = 4,
    .tx_invert = true,
    .rx_invert = false
};

LocoNetUartInterfaceHandle loconet_handle;
QueueHandle_t loconet_receive_queue;

void loconet_read_callback(LocoNetInterfaceHandle handle, LocoNetMessageRaw *message) {
    xQueueSendToBackFromISR(loconet_receive_queue, message, pdFALSE);
}

static void message_print_task(void *arg) {
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    LocoNetMessageRaw message;

    while (xQueueReceive(loconet_receive_queue, &message, (TickType_t)portMAX_DELAY)) {
        switch (message.opcode) {
            case (LOCONET_OPC_LOCO_SPD): {
                const LocoNetMessageLocoSpeed m = loconet_message_loco_speed_deserialize(&message);
                ESP_LOGI(TAG, "Slot: %d, Speed: %d", m.slot, m.speed);
                break;
            }
            
            case (LOCONET_OPC_SW_REQ): {
                const LocoNetMessageSwitchRequest m = loconet_message_switch_request_deserialize(&message);
                ESP_LOGI(TAG, "Switch: %d, Direction: %d, Output: %d", m.address, m.direction, m.output);
                break;
            }

            case (LOCONET_OPC_INPUT_REP): {
                const LocoNetMessageInputReport m = loconet_message_input_report_deserialize(&message);
                ESP_LOGI(TAG, "Input: %d, Source: %d, State: %d", m.address, m.source, m.state);
                break;
            }

            default: {
                ESP_LOGI(TAG, "Other Message:");
                ESP_LOG_BUFFER_HEXDUMP(TAG, &message, 4, ESP_LOG_INFO);
            }

        }
    }
}

void app_main(void) {
    loconet_handle = loconet_uart_start(loconet_uart_config);

    loconet_receive_queue = xQueueCreate(64, sizeof(LocoNetMessageRaw));
    if( loconet_receive_queue == NULL ) {
        /* Queue was not created and must not be used. */
        ESP_LOGE(TAG, "Failed to create LocoNet message receiving queue");
    } else {
        xTaskCreate(message_print_task, "loconet_rx_task", 1024 * 10, NULL, 3, NULL);
        loconet_interface_register_cb_read((LocoNetInterfaceHandle)loconet_handle, &loconet_read_callback);
    }    
}

