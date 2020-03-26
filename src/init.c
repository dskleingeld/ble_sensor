#include "init.h"

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    auto err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

void log_init(){
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();    
}
