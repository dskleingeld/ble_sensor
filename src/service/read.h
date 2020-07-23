#include <stdint.h>
#include "ble_gatts.h"

struct ReadState {
    ble_gatts_char_handles_t handle;
    uint16_t uuid;
    uint8_t data[20];
    bool notify_enabled;
};
extern struct ReadState read;

void add_read_characteristics(uint8_t base_index, uint16_t service_handle);