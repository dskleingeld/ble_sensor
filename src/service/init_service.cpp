/**
 * Copyright (c) 2013 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
extern "C" {
    #include "sdk_common.h"
    #include "ble_srv_common.h"
    #include "app_error.h"
}

#include "init_service.hpp"
#include <stdint.h>

/**@brief Function for handling the Write event.
 *
 * @param[in] p_lbs      LED Button Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_lbs_t* p_lbs, ble_evt_t const* p_ble_evt){
    /*ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (   (p_evt_write->handle == p_lbs->led_char_handles.value_handle)
        && (p_evt_write->len == 1)
        && (p_lbs->led_write_handler != NULL))
    {
        p_lbs->led_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_lbs, p_evt_write->data[0]);
    }*/
}

//enum BLE_GATTS_EVTS here for reference, do not uncomment
//{
//  BLE_GATTS_EVT_WRITE = BLE_GATTS_EVT_BASE, /**< Write operation performed.                                           \n See @ref ble_gatts_evt_write_t.                 */
//  BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST,       /**< Read/Write Authorization request.                                    \n Reply with @ref sd_ble_gatts_rw_authorize_reply. \n See @ref ble_gatts_evt_rw_authorize_request_t. */
//  BLE_GATTS_EVT_SYS_ATTR_MISSING,           /**< A persistent system attribute access is pending.                     \n Respond with @ref sd_ble_gatts_sys_attr_set.     \n See @ref ble_gatts_evt_sys_attr_missing_t.     */
//  BLE_GATTS_EVT_HVC,                        /**< Handle Value Confirmation.                                           \n See @ref ble_gatts_evt_hvc_t.                   */
//  BLE_GATTS_EVT_SC_CONFIRM,                 /**< Service Changed Confirmation.                                        \n No additional event structure applies.          */
//  BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST,       /**< Exchange MTU Request.                                                \n Reply with @ref sd_ble_gatts_exchange_mtu_reply. \n See @ref ble_gatts_evt_exchange_mtu_request_t. */
//  BLE_GATTS_EVT_TIMEOUT,                    /**< Peer failed to respond to an ATT request in time.                    \n See @ref ble_gatts_evt_timeout_t.               */
//  BLE_GATTS_EVT_HVN_TX_COMPLETE             /**< Handle Value Notification transmission complete.                     \n See @ref ble_gatts_evt_hvn_tx_complete_t.       */
//};

namespace Service {
    void on_ble_evt(ble_evt_t const* p_ble_evt, void* p_context)
    {
        ble_lbs_t * p_lbs = (ble_lbs_t *)p_context;

        switch (p_ble_evt->header.evt_id)
        {
            case BLE_GATTS_EVT_WRITE:
                on_write(p_lbs, p_ble_evt);
                break;

            default:
                // No implementation needed.
                break;
        }
    }


    ble_lbs_t init(ble_lbs_t* p_lbs) {
        uint32_t err_code;

        // Add service.
        ble_uuid128_t base_uuid = {LBS_UUID_BASE};
        err_code = sd_ble_uuid_vs_add(&base_uuid, &p_lbs->uuid_type);
        APP_ERROR_CHECK(err_code);

        ble_uuid_t ble_uuid;
        ble_uuid.type = p_lbs->uuid_type;
        ble_uuid.uuid = LBS_UUID_SERVICE;

        err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_lbs->service_handle);
        APP_ERROR_CHECK(err_code);

        return *p_lbs;
    }


    uint32_t on_button_change(uint16_t conn_handle, ble_lbs_t* p_lbs, uint8_t button_state)
    {
        ble_gatts_hvx_params_t params;
        uint16_t len = sizeof(button_state);

        memset(&params, 0, sizeof(params));
        params.type   = BLE_GATT_HVX_NOTIFICATION;
        //params.handle = p_lbs->button_char_handles.value_handle; //TODO change to our things
        params.p_data = &button_state;
        params.p_len  = &len;

        return sd_ble_gatts_hvx(conn_handle, &params);
    }
}