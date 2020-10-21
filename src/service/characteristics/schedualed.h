#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts.h"
#include "../sensors/timers.h"

struct SchedualedState {
    bool notify_enabled;
    uint16_t uuid;
    ble_gatts_char_handles_t handle;

    uint8_t data[3];
    struct Timer timer;
};
extern struct SchedualedState schedualed_state;
void enable_schedualed_notify(struct SchedualedState* self);
void disable_schedualed_notify(struct SchedualedState* self);


void add_schedualed_characteristics(uint8_t base_index, uint16_t service_handle);
