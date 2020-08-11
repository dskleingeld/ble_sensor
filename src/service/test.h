#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts.h"

struct TestState {
    ble_gatts_char_handles_t handle;
    uint16_t uuid;
    uint8_t data[20];
    bool notify_enabled;
};
extern struct TestState test_state;

void add_test_characteristics(uint8_t base_index, uint16_t service_handle);