#include "led_indicator.h"

#include "gateway.h"

const blink_step_t blink_off[] = {
    {LED_BLINK_HOLD, LED_STATE_OFF, 50},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_ON,  50},               // step1: turn on LED 50 ms
    {LED_BLINK_STOP, 0, 0},                           // step3: loop from step1
};

const blink_step_t blink_once[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_STOP, 0, 0},                           // step5: stop blink (off)
};

const blink_step_t blink_blink_slow[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 200},             // step2: turn off LED 100 ms
    {LED_BLINK_LOOP, 0, 0},                           // step3: loop from step1
};

const blink_step_t blink_blink_fast[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 50},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},             // step2: turn off LED 100 ms
    {LED_BLINK_LOOP, 0, 0},                           // step3: loop from step1
};

const blink_step_t blink_breath_fast[] = {
    {LED_BLINK_HOLD, LED_STATE_OFF, 0},                  // step1: set LED off
    {LED_BLINK_BREATHE, LED_STATE_ON, 500},              // step2: fade from off to on 500ms
    {LED_BLINK_BREATHE, LED_STATE_OFF, 500},             // step3: fade from on to off 500ms
    {LED_BLINK_LOOP, 0, 0},                              // step4: loop from step1
};

const blink_step_t blink_breath_slow[] = {
    {LED_BLINK_HOLD, LED_STATE_OFF, 0},                  // step1: set LED off
    {LED_BLINK_BREATHE, LED_STATE_ON, 1000},             // step2: fade from off to on 500ms
    {LED_BLINK_BREATHE, LED_STATE_OFF, 1000},            // step3: fade from on to off 500ms
    {LED_BLINK_LOOP, 0, 0},                              // step4: loop from step1
};

blink_step_t const * led_indicator_blink_lists[] = {
    [BLINK_OFF] = blink_off,
    [BLINK_ONCE] = blink_once,
    [BLINK_BLINK_SLOW] = blink_blink_slow,
    [BLINK_BLINK_FAST] = blink_blink_fast,
    [BLINK_BREATH_SLOW] = blink_breath_slow,
    [BLINK_BREATH_FAST] = blink_breath_fast,
};

led_indicator_ledc_config_t ledc_config = {
    .is_active_level_high = true,             
    .timer_inited = false,                      /*!< Set true if LEDC timer is inited, otherwise false. */
    .timer_num = LEDC_TIMER_0,                  /*!< The timer source of channel */
    .gpio_num = 19,                             /*!< num of gpio */
    .channel = LEDC_CHANNEL_0,                  /*!< LEDC channel */
};

led_indicator_ledc_config_t ledc2_config = {
    .is_active_level_high = true,             
    .timer_inited = false,                      /*!< Set true if LEDC timer is inited, otherwise false. */
    .timer_num = LEDC_TIMER_1,                  /*!< The timer source of channel */
    .gpio_num = 21,                             /*!< num of gpio */
    .channel = LEDC_CHANNEL_1,                  /*!< LEDC channel */
};

led_indicator_config_t loconet_led_config = {
    .mode = LED_LEDC_MODE,
    .led_indicator_ledc_config = &ledc_config,
    .blink_lists = led_indicator_blink_lists,
    .blink_list_num = 6,
};

led_indicator_config_t loconet_write_led_config = {
    .mode = LED_LEDC_MODE,
    .led_indicator_ledc_config = &ledc2_config,
    .blink_lists = led_indicator_blink_lists,
    .blink_list_num = 6,
};


led_indicator_handle_t loconet_led_handle;

void gateway_led_init()
{
    loconet_led_handle = led_indicator_create(&loconet_led_config);
    led_indicator_start(loconet_led_handle, BLINK_OFF);   
}

void gateway_led_loconet_activity(led_indicator_handle_t handle) {
    if (handle) {  
        const uint8_t brightness = led_indicator_get_brightness(handle);
        if (brightness > 0) {
            led_indicator_start(handle, BLINK_OFF);     
        }
    }
}