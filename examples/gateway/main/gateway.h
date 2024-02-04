#ifndef GATEWAY_H
#define GATEWAY_H

#include "led_indicator.h"

extern led_indicator_handle_t loconet_led_handle;

enum led_indicator_blink_type_t {
    BLINK_OFF,
    BLINK_ONCE,
    BLINK_BLINK_SLOW,   
    BLINK_BLINK_FAST,     
    BLINK_BREATH_SLOW,        
    BLINK_BREATH_FAST,               
};

void gateway_led_init();
void gateway_led_loconet_activity(led_indicator_handle_t handle);

#endif /* !GATEWAY_H */