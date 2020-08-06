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
    case PM_EVT_PEERS_DELETE_SUCCEEDED:
        advertising_start(false);
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
    uint8_t newkey[] = "123456";
    update_passkey(newkey);
    //randomize_passkey();
}

struct NonceState nonce = {
    .uuid = 4,
};
    
void add_nonce_characteristics(uint8_t base_index, uint16_t service_handle) {
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
    ble_uuid_t char_uuid = {nonce.uuid,base_index};
    attr_char_value.p_uuid = &char_uuid; //lifetime long enough?
    attr_char_value.p_attr_md = &attr_meta;
    attr_char_value.max_len = 16;
    attr_char_value.init_len = 16;
    attr_char_value.p_value = nonce.data;

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle,
        &char_meta,
        &attr_char_value,
        &nonce.handle);
    APP_ERROR_CHECK(err_code);
}

/*void test(){
    nrf_ecb_hal_data_t m_ecb_data; 
    uint8_t data[16] = {0};
    data[1] = 2;
    data[2] = 212;
    data[3] = 222;
    
    memcpy(&m_ecb_data.key[0], KEY, sizeof(KEY));
    memcpy(&m_ecb_data.cleartext[0], data, 16);

    NRF_LOG_INFO("cleartext:");
    for(int i = 0; i < 16; i++){
        NRF_LOG_INFO("%d", m_ecb_data.cleartext[i]);
    }

    sd_ecb_block_encrypt(&m_ecb_data);

    NRF_LOG_INFO("ciphertext:");
    for(int i = 0; i < 16; i++){
        NRF_LOG_INFO("%d", m_ecb_data.ciphertext[i]);
    }
}*/

void test(){
    nrf_crypto_aead_context_t ccm_ctx;
    nrf_crypto_aead_info_t const* p_ccm_k128_info = &g_nrf_crypto_aes_ccm_128_info; // for a 128-bit key
    uint32_t ret_val = nrf_crypto_aead_init(&ccm_ctx, p_ccm_k128_info, KEY);
    APP_ERROR_CHECK(ret_val);

    uint8_t data[16] = {0};
    data[1] = 2;
    data[2] = 212;
    data[3] = 222;
    uint8_t data_encrypted[16] = {0};
    uint8_t data_decrypted[16] = {0};
    uint8_t nonce[8] = {0};
    uint8_t mac[4] = {0};
    uint8_t adata[2] = {0};

    ret_val = nrf_crypto_aead_crypt(&ccm_ctx,
        NRF_CRYPTO_ENCRYPT,
        nonce, 
        sizeof(nonce),
        adata,//adata,
        sizeof(adata), //sizeof(adata),
        data,
        sizeof(data),
        data_encrypted,
        mac,
        sizeof(4)); //sizeof(mac));
    APP_ERROR_CHECK(ret_val);

    NRF_LOG_INFO("data_encrypted:");
    for(int i = 0; i < 16; i++){
        NRF_LOG_INFO("%d", data_encrypted[i]);
    }

    ret_val = nrf_crypto_aead_crypt(&ccm_ctx,
        NRF_CRYPTO_DECRYPT,
        nonce, 
        sizeof(nonce),
        adata,//adata,
        sizeof(adata), //sizeof(adata),
        data_encrypted,
        sizeof(data_encrypted),
        data_decrypted,
        mac,
        sizeof(mac)); //sizeof(mac));
    APP_ERROR_CHECK(ret_val);

    NRF_LOG_INFO("data_decrypted:");
    for(int i = 0; i < 16; i++){
        NRF_LOG_INFO("%d", data_decrypted[i]);
    }
}

void set_nonce_from_char(ble_evt_t const* p_ble_evt){
    uint8_t* data = p_ble_evt->evt.gatts_evt.params.write.data;
    nrf_ecb_hal_data_t m_ecb_data; 

    memcpy(&m_ecb_data.key[0], KEY, sizeof(KEY));
    memcpy(&m_ecb_data.cleartext[0], data, 16);
    
    NRF_LOG_INFO("cleartext:");
    for(int i = 0; i < 16; i++){
        NRF_LOG_INFO("%d", m_ecb_data.cleartext[i]);
    }

    sd_ecb_block_encrypt(&m_ecb_data);

    NRF_LOG_INFO("ciphertext:");
    for(int i = 0; i < 16; i++){
        NRF_LOG_INFO("%d", m_ecb_data.ciphertext[i]);
    }

    //retrieve response
    uint32_t pin;
    uint8_t response[6] = {0};
    memcpy(&response[0], &m_ecb_data.ciphertext[0], sizeof(response));    

    /*for(size_t i=0; i<sizeof(uint32_t); i++){
        const uint8_t bit_shifts = ( sizeof(uint32_t)-1-i ) * 8;
        pin |= (uint32_t)response[i] << bit_shifts;
    }*/

    for(size_t i=0; i<sizeof(uint32_t); i++){
        pin |= (uint32_t)response[i] << 8*i;
    }

    uint8_t new_key[12]; //prevent overflow allows full range of uint32_t
    sprintf((char*)new_key, "%ld", pin);
    NRF_LOG_INFO("new_key: %s", new_key);
    update_passkey(new_key);
}