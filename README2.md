# nRF5 stlink flash
based on: https://pcbreflux.blogspot.com/2016/09/nrf52832-first-steps-with-st-link-v2.html

- prerequisits
ninja, cmake, gcc-arm-none-eabi

- uninstall any openocd installed by apt
- pull openocd with nrf52 fixes https://github.com/marcelobarrosalmeida/openocd_nrf52
- follow instructions to make and install openocd with nrf52 fixes applied

- to monitor the output use screen /dev/ttyUSB0 115200 
(needs to be improved)

- using sdk version 16
- init setup,
run "cmake ../ -G Ninja"

- to build
run ninja from the build directory

 - changes to sdk_config.h:
NRF_SDH_BLE_VS_UUID_COUNT increased to 1
NRF_SDH_BLE_GATTS_ATTR_TAB_SIZE increased to 496

- how to connect for testing:
sudo bluetoothctl
    scan on
    connect C6:46:56:AC:2C:4C
    menu gatt
    list-attributes C6:46:56:AC:2C:4C
