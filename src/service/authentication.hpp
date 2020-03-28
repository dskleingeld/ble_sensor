//  https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/simple-application-level-authentication
#pragma once

extern "C" {
    #include "nrf_soc.h"
    #include "nrf_delay.h"
    #include "app_error.h"
    #include "ble_srv_common.h"
}

#include <cstring>
#include "keys.hpp"
#include "UUIDS.hpp"

// The RNG wait values are typical and not guaranteed. See Product Specifications for more info.
#define RNG_BYTE_WAIT_US       (124UL)
#define ECB_KEY_LEN            (16UL)

extern ble_gatts_char_handles_t auth_challange;
extern ble_gatts_char_handles_t auth_response;

namespace authentication {
    //needs to be declared extern as its defined in the implementation file
    extern uint8_t challange[16];
    extern bool authenticated;

    extern uint8_t unauthenticated_conn_hdl;
    //extern SoftwareTimer auto_disconnect_timer; 

    void setup(uint16_t service_handle, uint8_t uuid_type);
    void reset_auth();
    //void disconnect_unauthenticated(TimerHandle_t xTimerID);
    void randomise_challange();
    //void check_response(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len);
};