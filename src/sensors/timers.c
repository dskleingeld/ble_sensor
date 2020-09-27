#include "timers.h"
#include "nrf_log.h"

#include "../service/test.h"

void test_handler(void * p_context){
    NRF_LOG_INFO("TIMER FIRED");
}

static void init(struct Timer* timer){
    ret_code_t err_code = app_timer_create(timer->id,
        APP_TIMER_MODE_REPEATED,         
        timer->handler);
    APP_ERROR_CHECK(err_code);
}

void timers_init(){
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    init(&test_state.timer);
}

void timer_start(struct Timer timer){
    uint32_t timeout = APP_TIMER_TICKS(timer.timeout);
    ret_code_t err_code = app_timer_start(*timer.id, timeout, timer.handler);
    APP_ERROR_CHECK(err_code);
}

void timer_stop(struct Timer timer){
    ret_code_t err_code = app_timer_stop(*timer.id);
    APP_ERROR_CHECK(err_code);
}