//https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v16.0.0%2Fble_sdk_app_alert_notification.html

#include "nrf_log.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "ble_gatts.h"

#include "service_if.h"

#include "notify.h"
#include "read.h"
#include "write.h"

#include <stdint.h>

uint16_t connection_handle = BLE_CONN_HANDLE_INVALID;
ble_uuid128_t base_uuid = {{
    131, 20, 153, 220, 231, 245, 91, 
    152, 153, 21, 183, 27, 175, 191, 112, 147}};
uint16_t service_uuid = {0};

// Setup custom service UUID and different characteristics
uint32_t bluetooth_init() {
    uint32_t err_code;

    NRF_LOG_DEBUG("1.5")
    //add vendor specific base uuid
    uint8_t base_index; //nis set to the index of the added uuid
    err_code = sd_ble_uuid_vs_add(&base_uuid, &base_index);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEBUG("2")
    //add service with the above uuid
    uint16_t service_handle;
    ble_uuid_t ble_uuid = {service_uuid, base_index};
    err_code = sd_ble_gatts_service_add(
        BLE_GATTS_SRVC_TYPE_PRIMARY, 
        &ble_uuid, &service_handle);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEBUG("3")
    add_read_characteristics(base_index, service_handle);
    add_notify_characteristics(base_index, service_handle);
    add_write_characteristics(base_index, service_handle);
    NRF_LOG_DEBUG("4")
    return NRF_SUCCESS;
}

void handle_write(ble_evt_t const* p_ble_evt){
    ble_gatts_evt_write_t p_evt_write = p_ble_evt->evt.gatts_evt.params.write;
    uint16_t char_written_to = p_evt_write.handle;

    // Custom value CCCD written to and value is right length, i.e 2 bytes.
    if (char_written_to == notify.handle.cccd_handle 
        && p_evt_write.len == 2) {
        
        if(ble_srv_is_notification_enabled(p_evt_write.data)){
            NRF_LOG_INFO("notify enabled");
            notify.notify_enabled = true;
        } else {
            NRF_LOG_INFO("notify disabled");
            notify.notify_enabled = false;
        }
    } else if (char_written_to == write.handle.value_handle){
        NRF_LOG_INFO("written to characteristic");
    }
}

// Generic handler function for ble events will call each service's individual on_ble_evt function
void bluetooth_on_ble_evt(ble_evt_t const* p_ble_evt) {
    //NRF_LOG_INFO("got event")
    switch (p_ble_evt->header.evt_id){
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("connected");
            connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("disconnected");
            connection_handle = BLE_CONN_HANDLE_INVALID;
            notify.notify_enabled = false;
            break;
        case BLE_GATTS_EVT_WRITE:
            handle_write(p_ble_evt);
            break;
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            //auto n_completed = p_ble_evt->evt.gatts_evt.params.hvn_tx_complete.count;
            //NRF_LOG_INFO("TX COMPLETED");
            break;
        default:
            // No implementation needed.
            break;
    }
}

bool connected(){
    return connection_handle != BLE_CONN_HANDLE_INVALID;
}

// attribute-info 00002a05-0000-1000-8000-00805f9b34fb
//TODO send test data here
bool test_notify(uint8_t* data) {
    ble_gatts_hvx_params_t params;
    uint16_t len = 4;

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = notify.handle.value_handle;
    params.p_data = data;
    params.p_len  = &len;
    
    //https://github.com/NordicPlayground/nRF5x-custom-ble-service-tutorial

    if(notify.notify_enabled){
        uint32_t err_code = sd_ble_gatts_hvx(connection_handle, &params);
        switch(err_code) {
            case NRF_ERROR_RESOURCES:
                return false;
            //case BLE_ERROR_GATTS_SYS_ATTR_MISSING: //13313
            //    return true; //no idea why we do this
            default:
                break;
        }
        
        if (err_code != 0){
            NRF_LOG_ERROR(nrf_strerror_get(err_code));
            NRF_LOG_ERROR("%d", err_code);
            APP_ERROR_CHECK(err_code);
        }
    }
    return true;
}