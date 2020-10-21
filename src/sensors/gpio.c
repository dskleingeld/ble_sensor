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

void handle_gpio(uint8_t pin_no, uint8_t button_action){
    handle_button();
    handle_movement();
}

// has to be static for app_button to work
static app_button_cfg_t config[] = {
    {
    .pin_no = 30, 
    .active_state = APP_BUTTON_ACTIVE_HIGH,  
    .pull_cfg = NRF_GPIO_PIN_NOPULL, //NRF_GPIO_PIN_PULLDOWN, //NRF_GPIO_PIN_PULLUP,      
    .button_handler = handle_gpio,
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
