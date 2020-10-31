 ##############################################################################
# CMAKE CONFIGURATION
##############################################################################
cmake_minimum_required(VERSION 3.13)

set(PROJECT_NA "sensor")
project(ble_app_${PROJECT_NA} C ASM)

# set build type = Debug mode
set(CMAKE_BUILD_TYPE Debug)

message("\n" "=========================================")
message("Project: ${PROJECT_NAME} ")
message("=========================================")

set(CMAKE_C_STANDARD 99)

# You must set these variables with your own path
set(NRF_TARGET "nrf52") # nrf51 or nrf52
set(ARM_NONE_EABI_TOOLCHAIN_PATH "/usr") # Path to root folder 
set(NRF5_SDK_PATH "/home/kleingeld/Documents/Projects/ble_sensor/SDK_17") # Path to root folders
set(OPENOCD "openocd")	# Path to openocd if not in path
set(SOFTDEVICE "softdevice/s132/s132_nrf52_7.2.0_softdevice.hex")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include("${CMAKE_CURRENT_LIST_DIR}/cmake/nRF5x.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/arm-gcc-toolchain.cmake")

set(CMAKE_BUILD_TYPE Debug)

##############################################################################
# nRF5x setup (Comment/Uncomment libraries to use)
##############################################################################

# Libraries to use
nRF5x_setup()
nRF5x_addAppFIFO()
nRF5x_addAppTimer()
nRF5x_addAppUART()
nRF5x_addAppButton()
nRF5x_addBSP(TRUE FALSE FALSE) # (btn atn_btn nfc)
nRF5x_addBLEGATT()
nRF5x_addBLEService(ble_nus_c)
nRF5x_addBLEService(ble_lbs_c)
nRF5x_addBLEService(ble_lbs)
nRF5x_addAppFDS()
nRF5x_addBLEPeerManager()
nRF5x_addBLECrypto()
nRF5x_addBLEAdvertising()
nRF5x_addSensorSim()
nRF5x_addBLEDiscovery()
nRF5x_addBLEgq()
nRF5x_addBLEScan()
nRF5x_addAuthCrypt()
nRF5x_addAes()

# Libraries not used
# nRF5x_addAppScheduler()
# nRF5x_addNFC()

# -------------
# BLE SERVICES
# -------------
#  ble_ancs_c
#  ble_ans_c
#  ble_bas
#  ble_bas_c
#  ble_bps
#  ble_cscs
#  ble_cts_c
#  ble_dfu 
#  ble_dis 
#  ble_dis_c 
#  ble_escs 
#  ble_gls 
#  ble_hids
#  ble_hrs
#  ble_hrs_c
#  ble_hts
#  ble_ias 
#  ble_ias_c
#  ble_ipsp 
#  ble_lbs 
#  ble_lbs_c 
#  ble_lls 
#  ble_nus 
#  ble_nus_c 
#  ble_rscs 
#  ble_rscs_c 
#  ble_tps 
#  eddystone
#  nrf_ble_bms 
#  experimental...

add_definitions(-DCONFIG_GPIO_AS_PINRESET)


##############################################################################
# INCLUDE CONFIG FILE: sdk_config.h
##############################################################################
include_directories("${CMAKE_SOURCE_DIR}/config")

##############################################################################
# ERASE BOARD AND FLASH SOFTDEVICE
##############################################################################
# adds target for erasing and flashing the board with a softdevice
#add_custom_target(FLASH_ERASE
#        COMMAND ${OPENOCD} -d2 -f ../openocd_nrf52.cfg -c 'init_reset halt\; init\; nrf5 mass_erase\; init_reset run\; exit'
#        COMMENT "erasing flashing"
#        )

add_custom_target(FLASH_SOFTDEVICE
        COMMAND ${OPENOCD} -d2 -f ../openocd_nrf52.cfg -c 'program "../${SOFTDEVICE}" verify reset exit'
        COMMENT "flashing SoftDevice"
)

##############################################################################
# SOURCE CODE
##############################################################################

# own libs
include_directories(
        src, 
        src/service
        src/sensors
        dependencies/bitspec/c_src
)

list(APPEND SOURCE_FILES "src/service/service_if.c")
list(APPEND SOURCE_FILES "src/service/characteristics/dynamic.c")
list(APPEND SOURCE_FILES "src/service/characteristics/schedualed.c")

list(APPEND SOURCE_FILES "src/sensors/test_sensors.c")
list(APPEND SOURCE_FILES "src/sensors/timers.c")
list(APPEND SOURCE_FILES "src/sensors/gpio.c")
list(APPEND SOURCE_FILES "dependencies/bitspec/c_src/encoding.c")

list(APPEND SOURCE_FILES "src/pairing.c")
list(APPEND SOURCE_FILES "src/ble.c")
list(APPEND SOURCE_FILES "src/main.c")

#used std libs
list(APPEND STD_LIBS "m")

##############################################################################
# EXECUTABLES
##############################################################################
nRF5x_addExecutable(${PROJECT_NAME} "${SOURCE_FILES}")
target_link_libraries(${PROJECT_NAME} "${STD_LIBS}")


##############################################################################
# .hex, .map and .bin file 
##############################################################################
add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${PROJECT_NAME}.out
        COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.out "${PROJECT_NAME}.bin"
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.out "${PROJECT_NAME}.hex"
        COMMENT "post build steps for ${PROJECT_NAME}")

##############################################################################
# FLASHING PROGRAM TO BOARD 
##############################################################################
add_custom_target("FLASH_${PROJECT_NAME}" ALL
        DEPENDS ${PROJECT_NAME}
        COMMAND ${OPENOCD} -d2 -f ../openocd_nrf52.cfg -c 'program "${PROJECT_NAME}.hex" verify reset exit'
        COMMENT "flashing ${PROJECT_NAME}.hex")

message("=========================================")
message("Project: ${PROJECT_NAME} COMPILED WITH CMAKE " ${CMAKE_VERSION})
message("=========================================")
