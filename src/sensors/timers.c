#include "timers.h"
#include "nrf_log.h"

APP_TIMER_DEF(timer_1);
APP_TIMER_DEF(timer_2);

void test_handler(void * p_context){
    NRF_LOG_INFO("TIMER FIRED");
}

const struct Timer timers[] = {
    {id: &timer_1, timeout: 5000, handler: test_handler}
};

void timers_init(){
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.
    for(int i=0; i<sizeof(timers)/sizeof(struct Timer); i++){
        err_code = app_timer_create(timers[i].id,
            APP_TIMER_MODE_REPEATED,         
            timers[i].handler);
        APP_ERROR_CHECK(err_code);
    }
}


void sensor_timers_start(){
    ret_code_t err_code;
    for(int i=0; i<sizeof(timers)/sizeof(struct Timer); i++){
        uint32_t timeout = APP_TIMER_TICKS(timers[i].timeout);
        err_code = app_timer_start(*timers[i].id, timers[i].id, NULL);
        APP_ERROR_CHECK(err_code);
    }
}

void sensor_timers_stop(){
    ret_code_t err_code;
    for(int i=0; i<sizeof(timers)/sizeof(struct Timer); i++){
        err_code = app_timer_stop(*timers[i].id);
        APP_ERROR_CHECK(err_code);
    }
}