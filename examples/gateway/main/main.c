#include "esp_log.h"

#include "gateway.h"
#include "loconet.h"
#include "loconet_esp32.h"

#define QUEUE_SIZE 16

static const char *TAG = "main";

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

void loconet_write_callback(LocoNetInterfaceHandle handle, LocoNetMessageRaw *message) {
    gateway_led_loconet_activity(loconet_led_handle);
}

static void message_send_task(void *arg) {
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);

    // const LocoNetMessageLocoSpeed message = {
    //     .slot = 0x05,
    //     .speed = 50
    // };
    // LocoNetMessageRaw raw = loconet_message_loco_speed_serialize(&message);

    const LocoNetMessageSwitchRequest message = {
        .address = 65,
        .direction = LOCONET_SWITCH_DIRECTION_OPEN,
        .output = LOCONET_SWITCH_OUTPUT_OFF
    };
    LocoNetMessageRaw raw = loconet_message_switch_request_serialize(&message);
    
    while (1) {
        loconet_uart_message_send(loconet_handle, &raw);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void message_print_task(void *arg) {
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    LocoNetMessageRaw message;

    while (xQueueReceive(loconet_receive_queue, &message, (TickType_t)portMAX_DELAY)) {
        gateway_led_loconet_activity(loconet_led_handle);

        switch (message.opcode) {
            case (LOCONET_OPC_LOCO_SPD): {
                const LocoNetMessageLocoSpeed  parsed = loconet_message_loco_speed_deserialize(&message);
                ESP_LOGI(TAG, "Speed: %d, Slot: %d", parsed.speed, parsed.slot);
                break;
            }
            
            case (LOCONET_OPC_SW_REQ): {
                const LocoNetMessageSwitchRequest parsed = loconet_message_switch_request_deserialize(&message);
                ESP_LOGI(TAG, "Switch: %d, Direction: %d, Output: %d", parsed.address, parsed.direction, parsed.output);
                break;
            }

            case (LOCONET_OPC_LONG_ACK): {
                const LocoNetMessageLongAck parsed = loconet_message_long_ack_deserialize(&message);
                ESP_LOGI(TAG, "Long Ack, Opcode: %s, Response: %d", loconet_opcode_to_string(parsed.opcode), parsed.response);
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
    gateway_led_init();
    loconet_handle = loconet_uart_start(loconet_uart_config);

    
    loconet_receive_queue = xQueueCreate(64, sizeof(LocoNetMessageRaw));
    if( loconet_receive_queue == NULL ) {
        /* Queue was not created and must not be used. */
        ESP_LOGE(TAG, "Failed to create LocoNet message receiving queue");
    } else {
        xTaskCreate(message_print_task, "loconet_rx_task", 1024 * 10, NULL, 3, NULL);
        loconet_interface_register_cb_read((LocoNetInterfaceHandle)loconet_handle, &loconet_read_callback);
    }

    
    loconet_interface_register_cb_write((LocoNetInterfaceHandle)loconet_handle, &loconet_write_callback);
    xTaskCreate(message_send_task, "uart_tx_task", 1024 * 10, NULL, 3, NULL);
    
}

