#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_log_ctrl.h" //FLUSH LOG
#include "nrf_crypto.h"

#include "ble.h"
#include "sensors/gpio.h"
#include "sensors/timers.h"
#include "service_if.h"

#include "service/characteristics/schedualed.h"
#include "service/characteristics/dynamic.h"

typedef void* (*Task)();

Task tasks[2] = {
    init_schedualed, 
    init_dynamic,
};

int main() {
    log_init();
    timers_init();
    power_management_init();
    uint32_t ret_val = nrf_crypto_init();
    APP_ERROR_CHECK(ret_val);
    ret_val = nrf_crypto_rng_init(NULL, NULL);
    APP_ERROR_CHECK(ret_val);

    ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();

    services_init();
    conn_params_init();
    /* peer_manager_init(); */

    // Start execution.
    /* NRF_LOG_INFO("Bluetooth Dev Studio example started."); */
    /* application_timers_start(); */
    /* advertising_start(); */
    /* gpio_init(); */
    /* NRF_LOG_INFO("version 2.0"); */

    while(true) {
        idle_state_handle(); 
        /* for (int i=0; i < 2; i++) { */
        /*     void* next_task = tasks[i](); */
        /*     if (next_task != NULL) { */
        /*         tasks[i] = next_task; */
        /*     } */
        /* } */
        NRF_LOG_INFO("help");
        /* RTC_now(); */
        /* nrf_delay_ms(1000); */
    }
}
