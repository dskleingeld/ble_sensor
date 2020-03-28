extern "C" {//needed to prevent name wrangling
    #include "nrf_log.h"
    #include "nrf_delay.h"

    #include "init.h"
}

#include "setup_ble.hpp"
#include "millis.hpp"
#include "authentication.hpp"

/**@brief Function for application main entry.
 */
int main(void) {
    // Initialize.
    log_init();
    timers_init();
    millis_init();
    // buttons_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();

    authentication::setup();
    //queue.setup();
    //slow_measurements.setup();
    //fast_measurments.setup();

    // Start execution.
    NRF_LOG_INFO("Blinky example started.");
    NRF_LOG_FLUSH();
    advertising_start();

    NRF_LOG_INFO("flash %d", ticks_max());
    // Enter main loop.
    for (;;) {

        ////////////////////////////////////////////////////////
        /*if (client_subscribed) { //TODO re-enable
            slow_measurements.poll(queue);
            fast_measurments.poll(queue);
            queue.poll();
        }*/
        nrf_delay_ms(1000); //for debugging
        NRF_LOG_INFO("time: %d", ticks());
        NRF_LOG_FLUSH();
    }
}