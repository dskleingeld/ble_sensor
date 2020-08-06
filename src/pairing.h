#include <stdint.h>
#include "ble_gatts.h"

void peer_manager_init();
//void randomize_passkey();
void update_passkey(uint8_t newkey[]);
void init_passkey();

struct NonceState {
    ble_gatts_char_handles_t handle;
    uint16_t uuid;
    uint8_t data[16];
};
extern struct NonceState nonce;

void add_nonce_characteristics(uint8_t base_index, uint16_t service_handle);
void set_nonce_from_char();
void test();