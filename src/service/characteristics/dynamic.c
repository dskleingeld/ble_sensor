#include "dynamic.h"

#include "encoding.h"
#include "../sensors/test_sensors.h"

#include "nrf_log.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "ble_gatts.h"
#include "float.h"

static void handle_timer(void* p_context);

APP_TIMER_DEF(timer_id1);
struct DynamicState dynamic_state = {
    .uuid = 2,
    .notify_enabled = false,
    .timer = {id: &timer_id1, timeout: 500, handler: handle_timer},
};

void enable_dynamic_notify(struct DynamicState* self){
    NRF_LOG_INFO("notify enabled");
    self->notify_enabled = true;
    timer_start(self->timer);
}

void disable_dynamic_notify(struct DynamicState* self){
    NRF_LOG_INFO("notify disabled");
    self->notify_enabled = true;
    timer_stop(self->timer);
}

void add_dynamic_characteristics(uint8_t base_index, uint16_t service_handle) {
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
    ble_uuid_t char_uuid = {dynamic_state.uuid,base_index};
    attr_char_value.p_uuid = &char_uuid; //lifetime long enough?
    attr_char_value.p_attr_md = &attr_meta;
    attr_char_value.max_len = 3;
    attr_char_value.init_len = 3;
    attr_char_value.p_value = dynamic_state.data;

    NRF_LOG_INFO("uuid: %i", dynamic_state.uuid);
    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle,
        &char_meta,
        &attr_char_value,
        &dynamic_state.handle);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t p_uuid;
    ble_gatts_attr_md_t	p_md;
    sd_ble_gatts_attr_get(dynamic_state.handle.value_handle, &p_uuid, &p_md);
}

const union Field fields[] = {
	{.F32 = { // test button one
		.decode_add = 0,
		.decode_scale = 10,
		.length = 10,
		.offset = 0},
	},
	{.Bool = { // test movement sensor
		.offset = 10},
	},
};

static struct DynamicValue values[] = {
    { // sine
        .current = FLT_MIN,
        .maxdiff = 0.5,
        .last_send = FLT_MAX,
    },
    { // triangle
        .current = FLT_MIN,
        .maxdiff = 0.5,
        .last_send = FLT_MAX,
    },
}

extern uint16_t connection_handle;
bool send_notify(float value, union Field field){

    uint8_t data[3] = {0};
    uint16_t len = sizeof(data);
    ble_gatts_hvx_params_t params = {0};
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = dynamic_state.handle.value_handle;
    params.p_data = data;
    params.p_len  = &len;

    field.F32.offset = 0;
    encode_f32(&field, value, data);
    NRF_LOG_HEXDUMP_DEBUG(data,3);

    if(dynamic_state.notify_enabled){
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

static void handle_timer(void* p_context){
    values[0].current = measure_triangle();
    values[1].current = measure_sine();
    
    // only send it significant change
    for(int i=0; i<sizeof(values)/sizeof(DynamicValue); i++){
        float diff = fabs(values[i].last_send - values[0].current);
        if (diff > values[i].maxdiff) {
            send_notify(values[i], fields[i]);
        }
    }
}
