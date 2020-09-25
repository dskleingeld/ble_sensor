#pragma once

#include "app_timer.h"
#include <stdint.h>

struct Timer {
    const app_timer_id_t* id;
    uint32_t timeout; //in ticks
    app_timer_timeout_handler_t handler;
};

void test_handler(void * p_context);
void timers_init();
void timer_start(struct Timer timer);
void timer_stop(struct Timer timer);