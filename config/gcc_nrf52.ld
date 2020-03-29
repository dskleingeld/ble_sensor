/* Linker script to configure memory regions. */
/* 
  linker script how to:
  https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/4/html/Using_ld_the_GNU_Linker/script-format.html 
*/

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

/* MBR size: 4kB */
/*ram len: 32000 = 7D00*/
/*flash len: 256000 = 3E800*/
/*s113 MBR+softdevice usage: flash: 0x0001C000=114688, ram: 0x00001198=4504*/
/*s132 MBR+softdevice usage: flash: 0x00026000=155648, ram: 0x00001628=5672*/
/*set flash to free to*/
/*0x00000000 + (MBR+softdevice) flash size*/
/*and ram to*/
/*0x20000000 + (MBR+softdevice) ram size*/
/* more info: https://devzone.nordicsemi.com/nordic/short-range-guides/b/getting-started/posts/adjustment-of-ram-and-flash-memory */

/*with softdevic s132*/
/*ram starts at: 536876648, length left: 32768 = 0x8000*/
/*MEMORY*/
/*{*/
/*  FLASH (rx) : ORIGIN = 0x00026000, LENGTH = 0x50000*/
/*  RAM (rwx) : ORIGIN = 0x20001668, LENGTH = 0x8000*/
/*}*/

/*with softdevic s132 and extra softdevice ram*/
/*ram starts later then min s132 size (as instructed by runtime error)*/
/*length left smaller then min s132 (as instructed by runtime error)*/
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00026000, LENGTH = 0x50000
  RAM (rwx) : ORIGIN = 0x200022F0, LENGTH = 0xDD10
}

/*without any softdevice*/
/*ram len: 32000 = 7D00*/
/*flash len: 256000 = 3E800*/
/*MEMORY*/ 
/*{*/
/*  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x50000*/
/*  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000*/
/*}*/


/*seems to map certains symbols to places in ram where the softdevice will
  expect them */

SECTIONS
{
}

SECTIONS
{
  . = ALIGN(4);
  .mem_section_dummy_ram :
  {
  }
  .cli_sorted_cmd_ptrs :
  {
    PROVIDE(__start_cli_sorted_cmd_ptrs = .);
    KEEP(*(.cli_sorted_cmd_ptrs))
    PROVIDE(__stop_cli_sorted_cmd_ptrs = .);
  } > RAM
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
  .log_dynamic_data :
  {
    PROVIDE(__start_log_dynamic_data = .);
    KEEP(*(SORT(.log_dynamic_data*)))
    PROVIDE(__stop_log_dynamic_data = .);
  } > RAM
  .log_filter_data :
  {
    PROVIDE(__start_log_filter_data = .);
    KEEP(*(SORT(.log_filter_data*)))
    PROVIDE(__stop_log_filter_data = .);
  } > RAM

} INSERT AFTER .data;

SECTIONS
{
  .mem_section_dummy_rom :
  {
  }
  .sdh_soc_observers :
  {
    PROVIDE(__start_sdh_soc_observers = .);
    KEEP(*(SORT(.sdh_soc_observers*)))
    PROVIDE(__stop_sdh_soc_observers = .);
  } > FLASH
  .sdh_req_observers :
  {
    PROVIDE(__start_sdh_req_observers = .);
    KEEP(*(SORT(.sdh_req_observers*)))
    PROVIDE(__stop_sdh_req_observers = .);
  } > FLASH
  .sdh_state_observers :
  {
    PROVIDE(__start_sdh_state_observers = .);
    KEEP(*(SORT(.sdh_state_observers*)))
    PROVIDE(__stop_sdh_state_observers = .);
  } > FLASH
  .sdh_ble_observers :
  {
    PROVIDE(__start_sdh_ble_observers = .);
    KEEP(*(SORT(.sdh_ble_observers*)))
    PROVIDE(__stop_sdh_ble_observers = .);
  } > FLASH
  .pwr_mgmt_data :
  {
    PROVIDE(__start_pwr_mgmt_data = .);
    KEEP(*(SORT(.pwr_mgmt_data*)))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > FLASH
  .sdh_stack_observers :
  {
    PROVIDE(__start_sdh_stack_observers = .);
    KEEP(*(SORT(.sdh_stack_observers*)))
    PROVIDE(__stop_sdh_stack_observers = .);
  } > FLASH
  .log_const_data :
  {
    PROVIDE(__start_log_const_data = .);
    KEEP(*(SORT(.log_const_data*)))
    PROVIDE(__stop_log_const_data = .);
  } > FLASH
  .log_backends :
  {
    PROVIDE(__start_log_backends = .);
    KEEP(*(SORT(.log_backends*)))
    PROVIDE(__stop_log_backends = .);
  } > FLASH
    .nrf_balloc :
  {
    PROVIDE(__start_nrf_balloc = .);
    KEEP(*(.nrf_balloc))
    PROVIDE(__stop_nrf_balloc = .);
  } > FLASH

} INSERT AFTER .text


INCLUDE "nrf_common.ld"