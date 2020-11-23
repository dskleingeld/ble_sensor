#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts.h"
#include "../sensors/timers.h"

struct DynamicState {
    bool notify_enabled;
    uint16_t uuid;
    ble_gatts_char_handles_t handle;

    uint8_t data[3];
};

struct DynamicF32Val {
    float current;
    float last_send;
    const float maxdiff;
};

extern struct DynamicState dynamic_state;
void enable_dynamic_notify(struct DynamicState* self);
void disable_dynamic_notify(struct DynamicState* self);

void add_dynamic_characteristics(uint8_t base_index, uint16_t service_handle);
void* init_dynamic(uint32_t _now);
