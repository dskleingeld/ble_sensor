/* arduino millis like timer */
#pragma once

#include "app_timer.h"
#include "nrf_drv_clock.h"


/* setup and start the global millis timer */
void millis_init();
/* return numb of milliseconds since last overflow */
uint32_t millis();