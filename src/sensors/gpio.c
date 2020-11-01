#include "app_button.h"
#include "nrf_gpio.h"
#include "stdbool.h"
#include "nrf_log.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "timers.h"
#include "stdint.h"

// both come from service/characteristic/dynamic.c
extern void handle_dyn_button(uint32_t press_duration, uint8_t pin);
extern void handle_dyn_movement(bool pressed, uint8_t pin);

static uint32_t press_start;
void handle_button(uint8_t pin, uint8_t button_action){
    uint32_t press_duration;
    switch(button_action){
        case APP_BUTTON_PUSH:
            press_start = RTC_now();
            NRF_LOG_INFO("button activated");
            break;
        case APP_BUTTON_RELEASE:
            press_duration = RTC_elapsed(press_start);
            handle_dyn_button(press_duration, pin);
            NRF_LOG_INFO("button deactivated");
            break;
    }
}

void handle_movement_sens(uint8_t pin, uint8_t button_action){
    switch(button_action){
        case APP_BUTTON_PUSH:
            handle_dyn_movement(true, pin);
            NRF_LOG_INFO("movement sensor activated");
            break;
        case APP_BUTTON_RELEASE:
            handle_dyn_movement(false, pin);
            NRF_LOG_INFO("movement sensor deactivated");
            break;
    }
}

void handle_gpio(uint8_t pin_no, uint8_t button_action){
    switch(pin_no){
        case 2:
            handle_movement_sens(pin_no, button_action);
            break;
        case 31:
            handle_movement_sens(pin_no, button_action);
            break;
    }
}

// has to be static for app_button to work
static app_button_cfg_t config[] = {
    {
    .pin_no = 2, 
    .active_state = APP_BUTTON_ACTIVE_HIGH,  
    .pull_cfg = NRF_GPIO_PIN_NOPULL, // no pullup/down for MOVEMENT_SENS      
    .button_handler = handle_gpio,
    },
    {
    .pin_no = 31, 
    .active_state = APP_BUTTON_ACTIVE_HIGH,  
    .pull_cfg = NRF_GPIO_PIN_NOPULL, // no pullup/down for MOVEMENT_SENS
    .button_handler = handle_gpio,
    },
};

/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
void gpio_init() {
    uint32_t err_code = app_button_init(config, 1, 100);
    APP_ERROR_CHECK(err_code);
    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("gpio init done");
}
