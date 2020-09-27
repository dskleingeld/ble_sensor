#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts.h"
#include "../sensors/timers.h"

struct TestState {
    bool notify_enabled;
    uint16_t uuid;
    ble_gatts_char_handles_t handle;

    uint8_t data[3];
    struct Timer timer;
};
extern struct TestState test_state;
void enable_test_notify(struct TestState* self);
void disable_test_notify(struct TestState* self);


void add_test_characteristics(uint8_t base_index, uint16_t service_handle);