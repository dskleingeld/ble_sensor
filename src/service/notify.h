#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts.h"

struct NotifyState {
    ble_gatts_char_handles_t handle;
    uint16_t uuid;
    uint8_t data[20];
    bool notify_enabled;
};
extern struct NotifyState notify;

void add_notify_characteristics(uint8_t base_index, uint16_t service_handle);