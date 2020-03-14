// #include "nrf_log.h"
// #include "init_ble.h"
// #include "log.h"
// #include "nrf_delay.h"

// /**@brief Function for application main entry.
//  */
// int main(void) {


//     // Initialize.
//     log_init();
//     timers_init();
//     // buttons_init();
//     //power_management_init();
//     //ble_stack_init();
//     //gap_params_init();
//     //gatt_init();
//     //services_init();
//     //advertising_init();
//     //conn_params_init();

//     // Start execution.
//     NRF_LOG_INFO("Blinky example started.");
//     //advertising_start();

//     // Enter main loop.
//     for (;;)
//     {
//         //idle_state_handle();
//         //nrf_delay_ms(1000);
//         NRF_LOG_INFO("looploop.");
//     }
// }

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_log.h"
#include <nrfx_log.h>
#include <libraries/log/nrf_log_ctrl.h>
#include <libraries/log/nrf_log_default_backends.h>
#include "bsp.h"
#include "nrf.h"
#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "system_nrf52.h"
#include "nrf_delay.h"
#include "sdk_config.h"

#define ENABLE_SWD 1

static const uint8_t leds_list[2] = { 17,18 };

int main(void)
{

	NRF_LOG_INIT((void*)0);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    /* Configure LED-pins as outputs. */
    
    LEDS_CONFIGURE(LEDS_MASK);
    while (true)
    {
    	 NRF_LOG_PROCESS();
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            LEDS_INVERT(1 << leds_list[i]);
           
            nrf_delay_ms(500);
        }
        
        NRF_LOG_INFO("testing print CMAKE\n");
        NRF_LOG_FLUSH();
    }
}