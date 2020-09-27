#include <stdint.h>

#include "nrf_log.h"
#include "ble_conn_state.h"
#include "nrf_crypto.h"
#include <nrf_soc.h>

#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "ble.h"

#include "secrets.h"
#include "pairing.h"

static pm_peer_id_t m_peer_to_be_deleted = PM_PEER_ID_INVALID;
extern uint16_t m_conn_handle;

static void randomize_passkey(){
    uint8_t passkey[7] = {'\0'};

    uint8_t min = '0';
    uint8_t max = '9';
    for (int i=0; i<sizeof(passkey)-1; i++){
        uint32_t err_code = nrf_crypto_rng_vector_generate_in_range(&passkey[i], &min, &max, 1);
        APP_ERROR_CHECK(err_code);
    }

    ble_opt_t optS = {
        .gap_opt.passkey.p_passkey=&passkey[0],
    };

    uint32_t err_code = sd_ble_opt_set(BLE_GAP_OPT_PASSKEY, &optS);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id) {
    case PM_EVT_CONN_SEC_SUCCEEDED:
    {
        randomize_passkey();
        // Check if the link is authenticated (meaning at least MITM).
        pm_conn_sec_status_t conn_sec_status;
        err_code = pm_conn_sec_status_get(p_evt->conn_handle, &conn_sec_status);
        APP_ERROR_CHECK(err_code);

        if (conn_sec_status.mitm_protected) {
            NRF_LOG_INFO("Link secured. Role: %d. conn_handle: %d, Procedure: %d",
                ble_conn_state_role(p_evt->conn_handle),
                p_evt->conn_handle,
                p_evt->params.conn_sec_succeeded.procedure);
        } else {
            // The peer did not use MITM, disconnect.
            //NRF_LOG_INFO("Collector did not use MITM, disconnecting");
            err_code = pm_peer_id_get(m_conn_handle, &m_peer_to_be_deleted);
            APP_ERROR_CHECK(err_code);
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        }
    } break;
    case PM_EVT_CONN_SEC_FAILED:
        randomize_passkey();
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        break;
    default:
        break;
    }
}

void peer_manager_init() {
    uint32_t err_code;

    NRF_LOG_INFO("before init");
    err_code = pm_init();
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("after init");

    // Security parameters to be used for all security procedures.
    ble_gap_sec_params_t sec_param = {
        .bond           = false,
        .mitm           = true,
        .lesc           = true,
        .keypress       = false,
        .io_caps        = BLE_GAP_IO_CAPS_DISPLAY_ONLY, // or BLE_GAP_IO_CAPS_KEYBOARD_ONLY
        .oob            = false,
        .min_key_size   = 7,
        .max_key_size   = 16,
        .kdist_own.enc  = false,
        .kdist_own.id   = false,
        .kdist_peer.enc = false,
        .kdist_peer.id  = false,
    };

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

void update_passkey(uint8_t newkey[]){
    uint8_t passkey[7] = {'\0'};
    memcpy(passkey, newkey, sizeof(passkey)-1);
    ble_opt_t optS;
    optS.gap_opt.passkey.p_passkey=&passkey[0];
    uint32_t err_code = sd_ble_opt_set(BLE_GAP_OPT_PASSKEY, &optS);
    APP_ERROR_CHECK(err_code);
}

void init_passkey(){
    randomize_passkey();
}

struct NonceState nonce = {
    .uuid = NONCE_UUID,
};
    
void add_nonce_characteristics(uint8_t base_index, uint16_t service_handle) {
    //The Attribute Metadata: This is a structure holding permissions and 
    //authorization levels required by characteristic value attributes. 
    //It also holds information on whether or not the characteristic value 
    //is of variable length and where in memory it is stored.
    ble_gatts_attr_md_t attr_meta = {};
    attr_meta.vloc = BLE_GATTS_VLOC_USER;
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_meta.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_meta.write_perm);
    //The Characteristic Metadata: This is a structure holding the value 
    //properties of the characteristic value. It also holds metadata of the 
    //CCCD and possibly other kinds of descriptors.
    ble_gatts_char_md_t char_meta = {};
    char_meta.char_props.read = (uint8_t)false;
    char_meta.char_props.write = (uint8_t)true;
    char_meta.p_cccd_md = NULL; //Attribute metadata for the cccd, NULL for default values.
    //The Characteristic Value Attribute: This structure holds the actual value 
    //of the characteristic (like the temperature value). It also holds the 
    //maximum length of the value (it might e.g. be four bytes long) and it's UUID.
    ble_gatts_attr_t attr_char_value = {};
    ble_uuid_t char_uuid = {nonce.uuid,base_index};
    attr_char_value.p_uuid = &char_uuid; //lifetime long enough?
    attr_char_value.p_attr_md = &attr_meta;
    attr_char_value.max_len = 12;
    attr_char_value.init_len = 12;
    attr_char_value.p_value = nonce.data;

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle,
        &char_meta,
        &attr_char_value,
        &nonce.handle);
    APP_ERROR_CHECK(err_code);
}

struct PinState pin_state = {
    .uuid = PIN_UUID,
};

void add_pin_characteristics(uint8_t base_index, uint16_t service_handle) {
    //The Attribute Metadata: This is a structure holding permissions and 
    //authorization levels required by characteristic value attributes. 
    //It also holds information on whether or not the characteristic value 
    //is of variable length and where in memory it is stored.
    ble_gatts_attr_md_t attr_meta = {};
    attr_meta.vloc = BLE_GATTS_VLOC_STACK;
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_meta.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_meta.write_perm);
    //The Characteristic Metadata: This is a structure holding the value 
    //properties of the characteristic value. It also holds metadata of the 
    //CCCD and possibly other kinds of descriptors.
    ble_gatts_char_md_t char_meta = {};
    char_meta.char_props.read = (uint8_t)false;
    char_meta.char_props.write = (uint8_t)true;
    char_meta.p_cccd_md = NULL; //Attribute metadata for the cccd, NULL for default values.
    //The Characteristic Value Attribute: This structure holds the actual value 
    //of the characteristic (like the temperature value). It also holds the 
    //maximum length of the value (it might e.g. be four bytes long) and it's UUID.
    ble_gatts_attr_t attr_char_value = {};
    ble_uuid_t char_uuid = {pin_state.uuid,base_index};
    attr_char_value.p_uuid = &char_uuid; //lifetime long enough?
    attr_char_value.p_attr_md = &attr_meta;
    attr_char_value.max_len = 20;
    attr_char_value.init_len = 20;
    attr_char_value.p_value = pin_state.data;

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle,
        &char_meta,
        &attr_char_value,
        &pin_state.handle);
    APP_ERROR_CHECK(err_code);
}


void set_pin(ble_evt_t const* enc_key_event){
    uint8_t* enc_key_data = (uint8_t*)enc_key_event->evt.gatts_evt.params.write.data;
    uint8_t* enc_key_mac = (uint8_t*)enc_key_event->evt.gatts_evt.params.write.data+4;

    nrf_crypto_aead_context_t gcm_ctx;
    nrf_crypto_aead_info_t const* p_gcm_k128_info = &g_nrf_crypto_aes_gcm_128_info;
    uint32_t ret_val = nrf_crypto_aead_init(&gcm_ctx, p_gcm_k128_info, (uint8_t*)KEY);
    APP_ERROR_CHECK(ret_val);

    uint8_t pin_data[4] = {0};
    uint8_t adata[2] = {0};
    ret_val = nrf_crypto_aead_crypt(&gcm_ctx,
        NRF_CRYPTO_DECRYPT,
        nonce.data, 
        sizeof(nonce.data),
        adata, //adata,
        0, //sizeof(adata),
        enc_key_data,
        4, //sizeof(data_encrypted),
        pin_data,
        enc_key_mac,
        16); //sizeof(mac));
    
    // check for errors
    switch(ret_val) {
    case 0:
        break;
    case NRF_ERROR_CRYPTO_AEAD_INVALID_MAC:
        NRF_LOG_INFO("set pin has invalid mac");
        return;
    default:
        APP_ERROR_CHECK(ret_val);    
        return;
    }

    uint32_t pin = 0;
    for(size_t i=0; i<sizeof(pin); i++){
        const uint8_t bit_shifts = ( sizeof(pin)-1-i ) * 8;
        pin |= (uint32_t)pin_data[i] << bit_shifts;
    }

    uint8_t new_key[12]; //prevent overflow allows full range of uint32_t
    sprintf((char*)new_key, "%06lu", pin);
    NRF_LOG_INFO("new_key: %s", new_key);
    update_passkey(new_key);
}
