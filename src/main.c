#include "nrf_log.h"
#include "init_ble.h"
#include "log.h"

/**@brief Function for application main entry.
 */
int main(void) {


    // Initialize.
    log_init();
    timers_init();
    // buttons_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    // advertising_init();
    // conn_params_init();

    // Start execution.
    // NRF_LOG_INFO("Blinky example started.");
    // advertising_start();

    // Enter main loop.
    for (;;)
    {
        // idle_state_handle();
    }
}
