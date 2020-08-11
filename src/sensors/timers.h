#include "app_timer.h"

struct Timer {
    app_timer_id_t* id;
    uint32_t timeout; //in ticks
    app_timer_timeout_handler_t handler;
};

void timers_init();
void sensor_timers_start();
void sensor_timers_stop();