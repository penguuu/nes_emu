#include "nes.h"
#include "cartridge.h"
#include "debug.h"
#include "mapper.h"

__u8 load_cartridge_rom(char *fname){
	__u32 n;
	__u8 units;
	FILE *fp;

	fp = fopen(fname,"rb");

	printf("fname: %s\n",fname);
	debug_print("load_cartridge_rom: %s\n",fname);
	for(n=0;n<16;n++){
		header[n] = fgetc(fp);
	}

	if(header[0] != 'N' ||
	   header[1] != 'E' ||
	   header[2] != 'S' ||
           header[3] != 26){
		printf("Incorrect file header!\n");
		return 0;
	}
	else {
		debug_print("NES header found!\n");

		if(header[4] == 1)
			units = 2;
		else units = header[4];

		debug_print("Allocating %d*16384 (%d) bytes of memory for prg_rom\n",
			units,units*16384);
		
		prg_rom = malloc(units*16384);

		debug_print("prg_rom = %04X\n",prg_rom);
		debug_print("Allocating %d*8192 (%d) bytes of memory for chr_rom\n",
			header[5],header[5]*8192);
		chr_rom = malloc(header[5]*8192);

		prg_units = header[4];
		chr_units = header[5];

		if(header[6] & 0x8)
			mirroring = MIRROR_4SCREEN;
		else if(header[6] & 0x1)
			mirroring = MIRROR_VERTICAL;
		else
			mirroring = MIRROR_HORIZONTAL;
	
		debug_print("Reading prg-rom to buffer\n");
		for(n=0;n<header[4];n++){
			fread(prg_rom+(n*16384),16384,1,fp);
		}
		
		// if is just one unit long, make a copy of it to the end
		if(header[4] == 1){
			debug_print("memcpy() from: %04X to: %04X\n",prg_rom,prg_rom+16384);
			memcpy(prg_rom+16384,prg_rom,16384);
		}

		debug_print("Reading chr-rom to buffer\n");
		for(n=0;n<header[5];n++){
			fread(chr_rom+(n*8192),8192,1,fp);
		}
		fread(&title,128,1,fp);
		mirroring = MIRROR_VERTICAL;	
		debug_print("Returning from load_cartridge_rom()\n");
		return 1;
	}
}

__u8 unload_cartridge_rom(){
	free(prg_rom);
	free(chr_rom);
}
