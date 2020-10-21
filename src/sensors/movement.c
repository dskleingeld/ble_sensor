#include "movement.h"
#include "nrf_log.h"
#include "app_button.h"

void handle_movement(uint8_t pin_no, uint8_t button_action){
    switch(button_action){
        case APP_BUTTON_PUSH:
            NRF_LOG_INFO("movement sensor activated");
            break;
        case APP_BUTTON_RELEASE:
            NRF_LOG_INFO("movement sensor deactivated");
            break;
    }
}
