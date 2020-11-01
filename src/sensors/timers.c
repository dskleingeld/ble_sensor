#include "timers.h"
#include "app_timer.h"
#include "characteristics/dynamic.h"
#include "nrf_log.h"

#include "stdint.h"
#include "../service/characteristics/dynamic.h"
#include "../service/characteristics/schedualed.h"

extern struct Timer sht_timer;

void test_handler(void * p_context){
    NRF_LOG_INFO("TIMER FIRED");
}

static void init(struct Timer* timer){
    ret_code_t err_code = app_timer_create(timer->id,
        APP_TIMER_MODE_REPEATED,         
        timer->handler);
    APP_ERROR_CHECK(err_code);
}

static void init_ss(struct Timer* timer){
    ret_code_t err_code = app_timer_create(timer->id,
        APP_TIMER_MODE_SINGLE_SHOT,         
        timer->handler);
    APP_ERROR_CHECK(err_code);
}

void timers_init(){
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    init(&schedualed_state.timer);
    init_ss(&sht_timer);
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

// nRF5 RTC overflows after 512 seconds
// so we can not keep track of periods longer
// then that
uint32_t RTC_now() {
    return app_timer_cnt_get();
}

// handles overflow, returns time elapsed in milliseconds
uint32_t RTC_elapsed(uint32_t prev_ticks) {
    uint32_t now = RTC_now();
    uint32_t elapsed_ticks = (uint32_t)((int32_t)now - (int32_t)prev_ticks);
    uint32_t elapsed = elapsed_ticks / APP_TIMER_TICKS(1);
    return elapsed;
}
