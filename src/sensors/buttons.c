#include "buttons.h"

#include "app_button.h"
#include "nrf_gpio.h"
#include "stdbool.h"
#include "nrf_log.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "timers.h"

uint32_t pushed_at;

// TODO make this time the period a button is pressed
// (debouncing is already done)
void test_handler2(uint8_t pin_no, uint8_t button_action){
    uint32_t elapsed = 0;
    switch(button_action){
        case APP_BUTTON_PUSH:
            pushed_at = RTC_now();
            NRF_LOG_INFO("pushed");
            break;
        case APP_BUTTON_RELEASE:
            elapsed = RTC_elapsed(pushed_at);
            NRF_LOG_INFO("released");
            NRF_LOG_INFO("%d", elapsed);
            break;
    }
}

// has to be static for app_button to work
static app_button_cfg_t config[] = {
    {
    .pin_no = 30, 
    .active_state = APP_BUTTON_ACTIVE_HIGH,  
    .pull_cfg = NRF_GPIO_PIN_NOPULL, //NRF_GPIO_PIN_PULLDOWN, //NRF_GPIO_PIN_PULLUP,      
    .button_handler = test_handler2,
    }};

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
