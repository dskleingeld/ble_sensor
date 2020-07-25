#pragma once

void log_init();
void timers_init();
void buttons_leds_init(bool* p_erase_bonds);
void power_management_init();
void ble_stack_init();
void peer_manager_init();
void gap_params_init();
void gatt_init();
void advertising_init();
void services_init();
void conn_params_init();
void application_timers_start();
void advertising_start();
void idle_state_handle();