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

# debugging with gdb
start gdb server: sudo openocd -d2 -f openocd_nrf52.cfg
start gdb-multiarch: gdb-multiarch
then load the file: file build/ble_app_blinky.out
connect to server: tar remote localhost:3333
reset: mon reset init
mon reset halt
load info: load

# debugging bluetooth
use sudo btmon to monitor bluetooth traffic

# checking notify
sudo bluetoothctl
connect C6:46:56:AC:2C:4C
menu gatt
select-attribute 93700085-1bb7-1599-985b-f5e7dc991483
notify on

# DEV NOTES
seems to be crashing after some time

///////////////////////////////////////////////////////////////
out of bound authentication etc
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v7.0.1%2Fgroup___b_l_e___g_a_p___c_e_n_t_r_a_l___l_e_s_c___b_o_n_d_i_n_g___o_o_b___m_s_c.html
[mentions example] https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/nrf52832-and-android-nougat-simple-and-secure-touc
https://devzone.nordicsemi.com/f/nordic-q-a/14743/ble-minimal-modifications-to-use-pre-shared-key-auth-and-encryption
https://www.digikey.com/eewiki/display/Wireless/A+Basic+Introduction+to+BLE+Security#ABasicIntroductiontoBLESecurity-SecurityIssuesFacingBLE:

current plan:
    compile time set: public keys, confirmation values
    exchange in the clear: random nonces (investigate for possible security risks)

=> can not be done using the bluez dbus api. options:

- use constant passkey (security risc)
- use constant passkey + additional custom authentication scheme
- random passkey send over diy encrypted channel

* the additional authentication must be completed before passkey auth can be accepted. This should solve the MITM attack vector on passkey

* suggestion is to refuse further connections after x failures.
    -can set a variable and require reboot to try again or,
    -set a bit in flash that survives reboot to prevent power cycle attack


    https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v6.0.0%2Fstructble__gap__sec__kdist__t.html

    https://devzone.nordicsemi.com/f/nordic-q-a/35856/questions-about-lesc-mitm-and-passkey/138216#138216.
    combine with:
    https://en.wikipedia.org/wiki/Encrypted_key_exchange

    https://devzone.nordicsemi.com/f/nordic-q-a/51181/nrf_crypto_ecc_key_pair_generate-return-error/207121