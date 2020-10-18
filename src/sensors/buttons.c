#include "buttons.h"

#include "app_button.h"
#include "stdbool.h"
#include "nrf_log.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "timers.h"


/* const uint32_t pin = 30; */

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    /* switch(action) { */
    /*     case NRF_GPIOTE_POLARITY_LOTOHI: */
    /*         NRF_LOG_INFO("interrupt handler low high"); */
    /*         break; */
    /*     case NRF_GPIOTE_POLARITY_HITOLO: */
    /*         NRF_LOG_INFO("interrupt handler high low"); */
    /*         break; */
    /*     case NRF_GPIOTE_POLARITY_TOGGLE: */
    /*         if(high) { */
    /*             high = false; */
    /*         } else { */
    /*             high = true; */
    /*         } */
    /*         /1* NRF_LOG_INFO("interrupt handler toggle"); *1/ */
    /*         break; */
    /* } */
}

void test_handler2(uint8_t pin_no, uint8_t button_action){
    NRF_LOG_INFO("hi");
    switch(button_action){
        case APP_BUTTON_PUSH:
            NRF_LOG_INFO("pushed");
            break;
        case APP_BUTTON_RELEASE:
            NRF_LOG_INFO("released");
            break;
    }
}


static app_button_cfg_t config[] = {
    {
    .pin_no = 30, 
    .active_state = APP_BUTTON_ACTIVE_LOW,  
    .pull_cfg = NRF_GPIO_PIN_PULLUP,      
    .button_handler = test_handler2,
    }};

/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
void gpio_init() {
    uint32_t err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
    err_code = app_button_init(config, 1, 100);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("gpio init done");

    /* uint32_t err_code; */

    /* err_code = nrf_drv_gpiote_init(); */
    /* APP_ERROR_CHECK(err_code); */

    /* nrf_drv_gpiote_in_config_t in_config1 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true); */
    /* nrf_drv_gpiote_in_config_t in_config2 = GPIOTE_CONFIG_IN_SENSE_HITOLO(true); */
    /* in_config1.pull = NRF_GPIO_PIN_PULLUP; */
    /* in_config2.pull = NRF_GPIO_PIN_PULLUP; */

    /* err_code = nrf_drv_gpiote_in_init(pin, &in_config1, in_pin_handler); */
    /* APP_ERROR_CHECK(err_code); */
    /* err_code = nrf_drv_gpiote_in_init(pin, &in_config2, in_pin_handler); */
    /* APP_ERROR_CHECK(err_code); */

    /* nrf_drv_gpiote_in_event_enable(pin, true); */
}
