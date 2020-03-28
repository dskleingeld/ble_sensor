#include "millis.hpp"
#include <limits>
#include <stdbool.h>
#include "boards.h"
#include "bsp.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sdk_config.h"

//creates a timer "global_millis_timer" and statically
//allocates its memory
APP_TIMER_DEF(MILLIS_TIMER_ID);

static void timer_overflow_handler(void * p_context){}

static uint32_t ticks_to_millis(uint32_t ticks){
    uint64_t msec = ticks*2*1000/APP_TIMER_CLOCK_FREQ;
    return (uint32_t)msec;
}

static uint32_t ticks_to_micros(uint32_t ticks){
    uint64_t msec = ticks*2/APP_TIMER_CLOCK_FREQ;
    return (uint32_t)msec;
}

void millis_init(){
    ret_code_t err_code;
    err_code = app_timer_init();
    NRF_LOG_INFO("1.");
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&MILLIS_TIMER_ID, 
        APP_TIMER_MODE_REPEATED, timer_overflow_handler);
    NRF_LOG_INFO("2.");
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(MILLIS_TIMER_ID, 
        1000, NULL);
        //APP_TIMER_TICKS(timeout), NULL);
    NRF_LOG_INFO("3.");
    APP_ERROR_CHECK(err_code);
}

uint32_t ticks(){
    return app_timer_cnt_get();
}

uint32_t micros(){
    auto ticks = app_timer_cnt_get();
    return ticks_to_micros(ticks);
}

uint32_t millis(){
    auto ticks = app_timer_cnt_get();
    return ticks_to_millis(ticks);
}

uint32_t ticks_max(){
    return (1 << 24);
}

uint32_t micros_max(){
    return ticks_to_micros(ticks_max());
}

uint32_t millis_max(){
    return ticks_to_millis(ticks_max());
}