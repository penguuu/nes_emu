#include "nes.h"
#include "mem.h"
#include "cartridge.h"
#include "cpu.h"
#include "ppu.h"
#include "joystick.h"

__u8 read_mem(__u16 addr){
	__u16 real_addr;
	__u8 ret;

	/* memory mapper handler here */

	/* return from RAM */
	if(addr < 0x2000){
		real_addr = addr & 0x7FF;
		return ram[real_addr];
	}
	else if(addr < 0x4000){
		real_addr = 0x2000 + (addr & 0x7);
		ret = ppu_reg_read(real_addr);
		/* ppu register handling code here */
		return ret;
	}
	else if(addr < 0x4018){
		if(addr == 0x4016 || addr == 0x4017)
			ret = joystick_return_state(); 
		// joystick read code here
		// else:
//		ret = apu_read(addr);
		/* nes APU and IO-register handling code here */
		return ret;
	}
	else if(addr < 0x8000){
		/* Cartridge SRAM reading code here */
		return;
	}
	else {
		return *(prg_rom+(addr&0x7FFF));
		/* Read from PRG memory code here */
	}
}

/* TODO: mapper support */
void write_mem(__u16 addr, __u8 byte){
	int n;
	__u8 tmp;
	__u16 real_addr;
	/* memory mapper handler here */

	/* write to RAM */
	if(addr < 0x2000){
		real_addr = addr & 0x7FF;
		ram[real_addr] = byte;
		return;
	}
	/* write to ppu registers */
	else if(addr < 0x4000){
		real_addr = 0x2000 + (addr & 0x7);
		ppu_reg_write(real_addr,byte);
		/* ppu register handling code here */
		return;
	}
	else if(addr < 0x4018){
		// 0x4014 write transfer 256 bytes from written page to $2004
		// DMA access, transfer 256 bytes from $100 * byte
		// to $2004. not counting 0x4014 write tick it should take
		// 513 cpu cycles
		if(addr == 0x4014){
			for(n=0;n<256;n++){
				tmp = read_mem((byte<<8)+n);
				ppu_reg_write(0x2004,tmp);
			}
			ticks+=513;
		}
		if(addr == 0x4016)
			joystick_set_strobe(byte);
		// test if write is for joystick
		// else:
		//apu_write(addr, byte);
		/* NES APU and IO-register handling code here */
		return;
	}
	else if(addr < 0x8000){
		/* Cartridge SRAM writing code here */
		return;
	}	
}
