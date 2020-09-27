    -create bmp 
        hint: use stlink and st link utility (windows) to create one from a stm32
        then use dfu-util to complete flash
        
    
    -use explanation provided here to force install on 64k stm32: 
        https://microcontrollerelectronics.com/how-to-convert-an-stm32f103c8t6-into-a-black-magic-probe/
        (this unlocks the 128k mem, might not work perfectly)
        
--build BMP software
		Shell
		git clone --recursive https://github.com/blacksphere/blackmagic.git
		cd blackmagic
		make
		cd src
		make clean
		make PROBE_HOST=stlink

--flash
		make sure you are in the src dir.
		st-flash erase
		st-flash --flash=0x20000 write blackmagic_dfu.bin 0x8000000
		st-flash --flash=0x20000 --reset write blackmagic.bin 0x8002000
		
--use 
		make sure both boot jumpers are at 0
