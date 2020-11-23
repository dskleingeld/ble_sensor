#include "schedualed.h"
#include "encoding.h"
#include "../sensors/sht31.h"

#include "nrf_delay.h"
#include "nrf_log.h"
#include "app_timer.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "ble_gatts.h"
#include "timers.h"
#include <stdint.h>

struct SchedualedState schedualed_state = {
    .uuid = 1,
    .notify_enabled = false,
};

void enable_schedualed_notify(struct SchedualedState* self){
    NRF_LOG_INFO("sched notify enabled");
    self->notify_enabled = true;
}

void disable_schedualed_notify(struct SchedualedState* self){
    NRF_LOG_INFO("notify disabled");
    self->notify_enabled = false;
}

void add_schedualed_characteristics(uint8_t base_index, uint16_t service_handle) {
    //client characteristic config declaration
    ble_gatts_attr_md_t cccd_meta = {};
    cccd_meta.vloc = BLE_GATTS_VLOC_STACK; //value location
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_meta.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_meta.write_perm);
    //TODO FIXME re-enable: BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(&cccd_meta.write_perm);

    //The Attribute Metadata: This is a structure holding permissions and 
    //authorization levels required by characteristic value attributes. 
    //It also holds information on whether or not the characteristic value 
    //is of variable length and where in memory it is stored.
    ble_gatts_attr_md_t attr_meta = {};
    attr_meta.vloc = BLE_GATTS_VLOC_STACK;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_meta.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_meta.write_perm);
    //The Characteristic Metadata: This is a structure holding the value 
    //properties of the characteristic value. It also holds metadata of the 
    //CCCD and possibly other kinds of descriptors.
    ble_gatts_char_md_t char_meta = {};
    char_meta.char_props.read = (uint8_t)true;
    char_meta.char_props.notify = (uint8_t)true;
    char_meta.p_cccd_md = &cccd_meta;
    //The Characteristic Value Attribute: This structure holds the actual value 
    //of the characteristic (like the temperature value). It also holds the 
    //maximum length of the value (it might e.g. be four bytes long) and it's UUID.
    ble_gatts_attr_t attr_char_value = {};
    ble_uuid_t char_uuid = {schedualed_state.uuid,base_index};
    attr_char_value.p_uuid = &char_uuid; //lifetime long enough?
    attr_char_value.p_attr_md = &attr_meta;
    attr_char_value.max_len = 3;
    attr_char_value.init_len = 3;
    attr_char_value.p_value = schedualed_state.data;

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle,
        &char_meta,
        &attr_char_value,
        &schedualed_state.handle);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t p_uuid;
    ble_gatts_attr_md_t	p_md;
    sd_ble_gatts_attr_get(schedualed_state.handle.value_handle, &p_uuid, &p_md);
}

const union Field fields[] = {
	{.F32 = { // Temperature
		.decode_add = -20,
		.decode_scale = 0.1,
		.length = 10,
		.offset = 0},
	},
	{.F32 = { // Humidity
		.decode_add = 0,
		.decode_scale = 0.1,
		.length = 10,
		.offset = 10},
	},
};

extern uint16_t connection_handle;
bool send_notify(const float values[2]) {

    uint8_t data[3] = {0};
    uint16_t len = sizeof(data);
    ble_gatts_hvx_params_t params = {0};
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = schedualed_state.handle.value_handle;
    params.p_data = data;
    params.p_len  = &len;

    encode_f32(&fields[0], values[0], data);
    encode_f32(&fields[1], values[1], data); 
    NRF_LOG_HEXDUMP_DEBUG(data,3);

    if(schedualed_state.notify_enabled){
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

static void* start_measurements(uint32_t now);
static void* read_measurments(uint32_t now);
static void* send_measurments(uint32_t now);

static uint32_t last_run;
static float values[2]; //temp, humid
static void* start_measurements(uint32_t now) {
    uint32_t elapsed = RTC_elapsed(last_run);  
    NRF_LOG_INFO("start_measurements %d %d", last_run, elapsed);
    if(elapsed < 5000) {
        return NULL;
    }
    NRF_LOG_INFO("start 2");
    last_run = now; 
    sht31_request_temphum();
    return &read_measurments;
}

static void* read_measurments(uint32_t now) {
    NRF_LOG_INFO("read_measurments");
    if(RTC_elapsed(last_run) < 20) {
        return NULL;
    }
    NRF_LOG_INFO("read 2");
    sht31_read_temphum(&values[0], &values[1]);
    NRF_LOG_INFO("temp: " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(values[0]));
    NRF_LOG_INFO("hum: " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(values[1]));
    return &send_measurments;
}

static void* send_measurments(uint32_t now) {
    NRF_LOG_INFO("send 2");
    send_notify(values);
    return &start_measurements;
}

void* init_schedualed(uint32_t now) {
    twi_init();
    sht31_reset();
    last_run = now;
    return &start_measurements;
}
