/* arduino millis like timer */
#pragma once

#include "app_timer.h"
#include "nrf_drv_clock.h"
#include <cstdint>

/* setup and start the global millis timer */
void millis_init();
/* return numb of milliseconds since last overflow */
uint32_t millis();
uint32_t micros();
uint32_t ticks();

uint32_t millis_max();
uint32_t micros_max();
uint32_t ticks_max(); //nope experiments show somewhere around: 16680186 is the maximum is 2**24?