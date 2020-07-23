#include <stdint.h>
#include "ble_gatts.h"

struct WriteState {
    ble_gatts_char_handles_t handle;
    uint16_t uuid;
    uint8_t data[20];
};
extern struct WriteState write;

void add_write_characteristics(uint8_t base_index, uint16_t service_handle);