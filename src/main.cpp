extern "C" {
    #include "nrf_log.h"
    #include "nrf_delay.h"
    #include "nrf_log_ctrl.h" //FLUSH LOG
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
    NRF_LOG_INFO("version 1.0");

    // Enter main loop.
    uint32_t counter = 0;
    while (true) {
        //sends chip to sleep, dont want that now!
        //later we will use timers and will re-enable this
        idle_state_handle(); 
        //nrf_delay_ms(100); //needed to prevent nrf Queue from overflowing
        
        /*while(true){
            auto success = service::test_notify((uint8_t*)&counter);
            if(success){break;}
        }*/
        counter+=1;
        /*if (counter%20 == 0) {
            NRF_LOG_INFO("still alive");
            NRF_LOG_FLUSH();
        }*/
    }
}