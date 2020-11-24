#pragma once

#include <stdint.h>

void timers_init();

uint32_t RTC_elapsed(const uint32_t prev_ticks);
uint32_t RTC_now();
