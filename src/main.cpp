extern "C" {
    #include "nrf_log.h"
    #include "nrf_delay.h"
}

#include "ble.hpp"
#include "service_if.hpp"

int main() {

    // Initialize.
    log_init();
    timers_init();
    power_management_init();
    ble_stack_init();

    gap_params_init();
    gatt_init();
    advertising_init();
    services_init();
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("Bluetooth Dev Studio example started.");
    application_timers_start();
    advertising_start();

    // Enter main loop.
    uint32_t counter = 0;
    for (;;) {
        idle_state_handle();
        //nrf_delay_ms(1000);
        service::test_notify((uint8_t*)&counter);
        counter+=1;
    }
}