instructions:

works in 2 parts, first we unlock the chip then we flash the bootloader.

Unlocking
    -install gcc-arm-none-eabi (to add this arch to gdb)
    
    -connect nrf52832 to black magic probe [bmp] (see folder create_bmp)
        PA5 - SWDCLK
        PB14 - SWDIO
        3V3 - 3V3
        GND - GND
        
    -connect bmp-usb to pc (in this order)
    
    -run gdb and input:
        target extended-remote /dev/ttyACM0 (bmp port, check it using dmesg)
        mon swdp_scan
        attach # (here # is the number for the Nordic nRF52 Access Port)
        mon erase_mass
    
    -if chip is unlocked a second driver should show after mon swdp_scan named:
        Nordic nRF52

see: https://github.com/AndruPol/nrf52832-recover/blob/master/README.md
see: https://github.com/blacksphere/blackmagic/issues/230
