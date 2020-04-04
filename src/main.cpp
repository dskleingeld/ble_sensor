extern "C" {
    #include "nrf_log.h"
}

#include "ble.hpp"

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
    for (;;)
    {
        idle_state_handle();
    }
}