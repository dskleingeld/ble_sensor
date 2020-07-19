//https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v16.0.0%2Fble_sdk_app_alert_notification.html

extern "C" {
    #include "nrf_log.h"
    #include "app_error.h"
    #include "ble_srv_common.h"
    #include "ble_gatts.h"
}

#include "service_if.hpp"
#include <cstdint>

namespace link_test {
    ble_gatts_char_handles_t handle;
    constexpr uint16_t uuid = 42;
    uint8_t data[20];
    bool notify_enabled = false;
}

namespace service {
    uint16_t connection_handle = BLE_CONN_HANDLE_INVALID;
    ble_uuid128_t base_uuid = {
        131, 20, 153, 220, 231, 245, 91, 
        152, 153, 21, 183, 27, 175, 191, 112, 147};
    uint16_t service_uuid = {0};

    //TODO rewrite using: https://devzone.nordicsemi.com/nordic/short-range-guides/b/bluetooth-low-energy/posts/ble-characteristics-a-beginners-tutorial

    // Setup custom service UUID and different characteristics
    uint32_t bluetooth_init() {
        uint32_t err_code;

        //add vendor specific base uuid
        uint8_t base_index; //nis set to the index of the added uuid
        err_code = sd_ble_uuid_vs_add(&base_uuid, &base_index);
        APP_ERROR_CHECK(err_code);

        
        //add service with the above uuid
        uint16_t service_handle;
        auto ble_uuid = ble_uuid_t {service_uuid, base_index};
        err_code = sd_ble_gatts_service_add(
            BLE_GATTS_SRVC_TYPE_PRIMARY, 
            &ble_uuid, &service_handle);
        APP_ERROR_CHECK(err_code);

    
        //client characteristic config declaration
        ble_gatts_attr_md_t cccd_meta;
        memset(&cccd_meta, 0, sizeof(cccd_meta));
        cccd_meta.vloc = BLE_GATTS_VLOC_STACK; //value location
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_meta.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_meta.write_perm);

        //This is a structure holding permissions and 
        //authorization levels required by characteristic value attributes.
        ble_gatts_attr_md_t attr_meta;
        memset(&attr_meta, 0, sizeof(attr_meta));
        attr_meta.vloc = BLE_GATTS_VLOC_STACK;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_meta.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_meta.write_perm);
        //This is a structure holding the value 
        //properties of the characteristic value.
        ble_gatts_char_md_t char_meta;
        memset(&char_meta, 0, sizeof(char_meta));
        char_meta.char_props.read = (uint8_t)true;
        char_meta.char_props.notify = (uint8_t)true;
        char_meta.p_cccd_md = &cccd_meta;

        ble_uuid_t char_uuid = {link_test::uuid,base_index};
        //This structure holds the actual value of the characteristic 
        //(like the temperature value).
        ble_gatts_attr_t attr_char_value;
        memset(&attr_char_value, 0, sizeof(attr_char_value));
        attr_char_value.p_uuid = &char_uuid; //lifetime long enough?
        attr_char_value.p_attr_md = &attr_meta;
        attr_char_value.max_len = 8;
        attr_char_value.init_len = 8;
        attr_char_value.p_value = link_test::data;

        err_code = sd_ble_gatts_characteristic_add(service_handle,
            &char_meta,
            &attr_char_value,
            &link_test::handle);
        APP_ERROR_CHECK(err_code);

        return NRF_SUCCESS;
    }

    void handle_write(ble_evt_t const* p_ble_evt){
        auto p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
        auto char_written_to = p_evt_write->handle;
        
            // Custom value CCCD written to and value is right length, i.e 2 bytes.
        if (char_written_to == link_test::handle.cccd_handle 
            && p_evt_write->len == 2) {
            
            if(ble_srv_is_notification_enabled(p_evt_write->data)){
                NRF_LOG_INFO("notify enabled");
                link_test::notify_enabled = true;
            } else {
                NRF_LOG_INFO("notify disabled");
                link_test::notify_enabled = false;
            }
        }
    }

    // Generic handler function for ble events will call each service's individual on_ble_evt function
    void bluetooth_on_ble_evt(ble_evt_t const* p_ble_evt) {
        //NRF_LOG_INFO("got event")
        switch (p_ble_evt->header.evt_id){
            case BLE_GAP_EVT_CONNECTED:
                NRF_LOG_INFO("connected");
                service::connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
                break;
            case BLE_GAP_EVT_DISCONNECTED:
                NRF_LOG_INFO("disconnected");
                service::connection_handle = BLE_CONN_HANDLE_INVALID;
                link_test::notify_enabled = false;
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
        return service::connection_handle != BLE_CONN_HANDLE_INVALID;
    }

    // attribute-info 00002a05-0000-1000-8000-00805f9b34fb
    //TODO send test data here
    bool test_notify(uint8_t* data) {
        ble_gatts_hvx_params_t params;
        uint16_t len = 4;

        memset(&params, 0, sizeof(params));
        params.type   = BLE_GATT_HVX_NOTIFICATION;
        params.handle = link_test::handle.value_handle;
        params.p_data = data;
        params.p_len  = &len;
        
        //https://github.com/NordicPlayground/nRF5x-custom-ble-service-tutorial

        if(link_test::notify_enabled){
            auto err_code = sd_ble_gatts_hvx(service::connection_handle, &params);
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
}