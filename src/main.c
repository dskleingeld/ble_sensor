#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_log_ctrl.h" //FLUSH LOG
#include "nrf_crypto.h"

#include "ble.h"
#include "service_if.h"
#include "timers.h"

int main() {

    // Initialize.
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
    peer_manager_init();

    // Start execution.
    NRF_LOG_INFO("Bluetooth Dev Studio example started.");
    application_timers_start();
    advertising_start();
    sensor_timers_start();
    NRF_LOG_INFO("version 2.0");

    // Enter main loop.
    //uint32_t i = 0;
    for (;;){
        //sends chip to sleep, dont want that now!
        //later we will use timers and will re-enable this
        idle_state_handle(); 
        //nrf_delay_ms(100); //needed to prevent nrf Queue from overflowing
        
        /*for (;;){
            bool success = test_notify((uint8_t*)&i);
            if(success){break;}
        }
        
        i+=1;
        if (i%20 == 0) {
            NRF_LOG_INFO("still alive");
            NRF_LOG_FLUSH();
        }*/
    }
}