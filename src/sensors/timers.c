#include "timers.h"
#include "characteristics/dynamic.h"
#include "nrf_log.h"

#include "nrfx_rtc.h"
#include "stdint.h"

static const nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

static void rtc_handler(nrfx_rtc_int_type_t int_type){}

void timers_init(){
    const uint32_t RTC_FREQ = 2048;
    nrfx_rtc_config_t config = {
        .prescaler = RTC_FREQ_TO_PRESCALER(RTC_FREQ),
        .interrupt_priority = 6,
        .reliable = false,
        .tick_latency = NRFX_RTC_US_TO_TICKS(5, RTC_FREQ),
    };
    NRF_LOG_INFO("before init");
    uint32_t err = nrfx_rtc_init(&rtc, &config, &rtc_handler);
    NRF_LOG_INFO("after init %d", err);
    APP_ERROR_CHECK(err);

    nrfx_rtc_overflow_enable(&rtc, false);
    nrfx_rtc_enable(&rtc);
}


// nRF5 RTC overflows after 512 seconds
// so we can not keep track of periods longer
// then that
uint32_t RTC_now() {
    uint32_t now = nrfx_rtc_counter_get(&rtc);
    NRF_LOG_INFO("now: %d", now);
    return now;
}

// handles overflow, returns time elapsed in milliseconds
uint32_t RTC_elapsed(const uint32_t prev_ticks) {
    uint32_t now = RTC_now();
    NRF_LOG_INFO("prev_ticks: %d, now: %d", prev_ticks, now);
    uint32_t elapsed_ticks = (uint32_t)((int64_t)now - (int64_t)prev_ticks);
    /* uint32_t elapsed = elapsed_ticks / NRFX_RTC_US_TO_TICKS(1000); */
    uint32_t elapsed = elapsed_ticks;
    return elapsed;
}
