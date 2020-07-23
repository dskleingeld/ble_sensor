#include <stdbool.h>
#include <stddef.h>

#include "app_error.h"

#include "write.h"

struct WriteState write = {
    .uuid = 3,
};
    
void add_write_characteristics(uint8_t base_index, uint16_t service_handle) {
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
    ble_uuid_t char_uuid = {write.uuid,base_index};
    attr_char_value.p_uuid = &char_uuid; //lifetime long enough?
    attr_char_value.p_attr_md = &attr_meta;
    attr_char_value.max_len = 4;
    attr_char_value.init_len = 4;
    attr_char_value.p_value = write.data;

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle,
        &char_meta,
        &attr_char_value,
        &write.handle);
    APP_ERROR_CHECK(err_code);
}