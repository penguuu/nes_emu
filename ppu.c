#include "nes.h"
#include "mapper.h"
#include "ppu.h"
#include "render.h"
#include "cpu.h"
#include "cartridge.h"
#include "palette.h"

__u8 ppu_reg_read(__u16 addr){
	__u8 res = 0;

	switch(addr){
		case 0x2002:
			if(ppu.vblank_active){
				res |= 0x80;
				ppu.vblank_active = 0;				
			}
			if(ppu.sprite0_hit){
				res |= 0x40;
			}
			if(ppu.sprite_overflow)
				res |= 0x20;

			ppu.addr_latch = 0;
			break;
		case 0x2004:
			res = ppu.sprite_ram[ppu.oam_addr];
			break;
		case 0x2007:
			res = ppu_vram_read(ppu.vram_addr);
			ppu.vram_addr += ppu.vram_addr_increment;	
			ppu.vram_addr &= 0x3FFF;
			break;	
	}

	return res;
}

void ppu_reg_write(__u16 addr, __u8 byte){
	switch(addr){
		case 0x2000:
			if(byte & 0x80) 
				ppu.nmi_on_vblank = 1;
			else 
				ppu.nmi_on_vblank = 0;

			if(byte & 0x40) 
				ppu.ppu_master_slave_select = 1;
			else 
				ppu.ppu_master_slave_select = 0;

			if(byte & 0x20) 
				ppu.sprite_size = 16;
			else 
				ppu.sprite_size = 8;

			if(byte & 0x10) 
				ppu.bg_pattern_tbl_addr = 0x1000;
			else
				ppu.bg_pattern_tbl_addr = 0x0000;

			if(byte & 0x8)
				ppu.sprite_ptrn_tbl_addr = 0x1000;
			else
				ppu.sprite_ptrn_tbl_addr = 0x0000;
	
			if(byte & 0x4)
				ppu.vram_addr_increment = 32;
			else
				ppu.vram_addr_increment = 1;

			ppu.vram_addr_tmp = (ppu.vram_addr_tmp & ~(3 << 10)) | ((byte & 3) << 10);
			/*
			if((byte & 0x2) && (byte & 0x1))
				ppu.base_nametable_addr = 0x2C00;
			else if((byte & 0x2) && !(byte & 0x1))
				ppu.base_nametable_addr = 0x2800;
			else if(!(byte & 0x2) && (byte & 0x1))
				ppu.base_nametable_addr = 0x2400;
			else
				ppu.base_nametable_addr = 0x2000;*/
			break;

		case 0x2001:
			if(byte & 0x80)
				ppu.intensify_blue = 1;
			else
				ppu.intensify_blue = 0;

			if(byte & 0x40)
				ppu.intensify_green = 1;
			else
				ppu.intensify_green = 0;

			if(byte & 0x20)
				ppu.intensify_red = 1;
			else
				ppu.intensify_red = 0;

			if(byte & 0x10)
				ppu.show_sprites = 1;
			else
				ppu.show_sprites = 0;

			if(byte & 0x8)
				ppu.show_background = 1;
			else
				ppu.show_background = 0;

			if(byte & 0x4)
				ppu.show_sprites_left_8 = 1;
			else
				ppu.show_sprites_left_8 = 0;

			if(byte & 0x2)
				ppu.show_bg_left_8 = 1;
			else
				ppu.show_bg_left_8 = 0;

			if(byte & 0x1)
				ppu.grayscale = 1;
			else
				ppu.grayscale = 0;

			break;
/*
		case 0x2002:
			ppu.status = byte;
			break;
*/
		case 0x2003:
			ppu.oam_addr = byte;
			break;
		case 0x2004:
			ppu.sprite_ram[ppu.oam_addr++] = byte;
			break;
		case 0x2005:
			if(ppu.addr_latch == 0){
				//ppu.scroll_latch = 1;
				ppu.addr_latch = 1;
				ppu.vram_addr_tmp = (ppu.vram_addr_tmp & ~0x1F) | ((byte >> 3) & 0x1F);
				ppu.x_scroll = byte & 7;
			}
			else {
				//ppu.scroll_latch = 0;
				ppu.addr_latch = 0;
				ppu.y_scroll = byte;
				ppu.vram_addr_tmp = (ppu.vram_addr_tmp & ~(0x1F << 5)) | (((byte >> 3) & 0x1F) << 5);
				ppu.vram_addr_tmp = (ppu.vram_addr_tmp & ~(7 << 12)) | ((byte & 7) << 12);
			}
			break;
		case 0x2006:
			if(ppu.addr_latch == 0){
				ppu.addr_latch = 1;
				ppu.vram_addr_tmp = (ppu.vram_addr_tmp & 0xFF) | ((__u16) (byte&0x3F) << 8);
			}
			else {
				ppu.addr_latch = 0;
				ppu.vram_addr_tmp = (ppu.vram_addr_tmp & ~0xFF) | (__u16) byte;
				ppu.vram_addr = ppu.vram_addr_tmp;
			}
			break;
		case 0x2007:
			ppu_vram_write(ppu.vram_addr,byte);
			ppu.vram_addr += ppu.vram_addr_increment;
			ppu.vram_addr &= 0x3FFF;
			break;	
	}
}

void ppu_vram_write(__u16 addr, __u8 byte){
	addr = handle_addr_mirror(addr);
	ppu.vram[addr] = byte;
}

__u8 ppu_vram_read(__u16 addr){
	if(addr < 0x2000){
		return chr_rom[addr]; 
	}
	addr = handle_addr_mirror(addr);
	
	return ppu.vram[addr];
}

void ppu_tick(__u16 cpu_ticks) {
	__u16 ppu_ticks;
	_rgb p; 

	ppu_ticks = (cpu_ticks * PPU_PER_CPU)-1;
	ppu.cycle_count += ppu_ticks;

	if(ppu.cycle_count >= CYCLES_PER_SCANLINE){
		ppu.scanline += 1;
		ppu.cycle_count = ppu.cycle_count-CYCLES_PER_SCANLINE;
	}

	/* normal render */
	if(ppu.scanline >= 0 && ppu.scanline <= 239){
		if(ppu.scanline != ppu.prev_scanline){
			if(ppu.show_sprites == 1)
				render_sprites(0,ppu.sprite_size);
			if(ppu.show_background == 1)
				render_bg();
			if(ppu.show_sprites == 1)
				render_sprites(1,ppu.sprite_size);

			if(ppu.show_sprites == 1 || ppu.show_background == 1){
				ppu.vram_addr = (ppu.vram_addr & (~0x1F & ~(1 << 10))) |
				(ppu.vram_addr_tmp & (0x1F | (1 << 10)));
			}

/*			if(ppu.show_sprites == 1 && ppu_show_background == 1){
				ppu.vram_addr = (ppu.vram_addr & (~0x1F & ~(1 << 10))) | (ppu.vram_latch & (0x1F | (1 << 10)));
				// irq();
			}
*/	
			ppu.prev_scanline = ppu.scanline;
		}
	}
	else if(ppu.scanline == 240){
		// post render, ppu idles this scanline
	}
	/* vblank */
	else if(ppu.scanline == 241){
		ppu.vblank_active = 1;
		/* nmi on vblank */
		if(ppu.nmi_on_vblank == 1){
			nmi_in_queue++;
			ppu.vblank_nmi_done = 1;			
		}
	}

	else if(ppu.scanline == 260){
		ppu.vblank_active = 0;
	}
	/* last scanline */
	else if(ppu.scanline == 261){
		ppu.vblank_nmi_done = 0;			
		ppu.scanline = -1;
		ppu.prev_scanline = -1;
		ppu.sprite0_hit = 0;
		ppu.sprite_overflow = 0;
		ppu.addr_latch = 0;

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);

		diff = calc_diff(&now,&last);

		// 16666666 = 60Hz
		if(diff.tv_nsec < 16666666){
			diff.tv_sec = 0;
			diff.tv_nsec = 16666666 - diff.tv_nsec;
			nanosleep(NULL,&diff);
		}

		p.r=0;p.g=0;p.b=0;
		update_screen(p);
		
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&last);
		if(ppu.show_sprites == 1 || ppu.show_background == 1)
			ppu.vram_addr = ppu.vram_addr_tmp;
	}

	ppu.cycle_count++;
}

struct timespec calc_diff(struct timespec *t1, struct timespec *t2){
	struct timespec ret;

	if(t1->tv_sec > t2->tv_sec && t1->tv_nsec >= t2->tv_nsec){
		ret.tv_sec = t1->tv_sec - t2->tv_sec;
		ret.tv_nsec = t1->tv_nsec - t2->tv_nsec;
			
	}
	else if(t1->tv_sec > t2->tv_sec && t1->tv_sec < t2->tv_nsec){
		ret.tv_sec = 0;
		ret.tv_nsec = ((1000000000)+t1->tv_sec) - t2->tv_nsec;
	}
	else if(t1->tv_sec == t2->tv_sec && t1->tv_nsec > t2->tv_nsec) {
		ret.tv_sec = 0;
		ret.tv_nsec = t1->tv_nsec - t2->tv_nsec;
	}

	return ret;
}

/* t = 0 front of bg, t 1 == back */
void render_sprites(__u8 t, __u8 size){
	int n,l;
	__u8 tile_idx, attrib, x,y, range;

	ppu.sprites_on_scanline = 0;

	for(n=63;n>=0;n--){
		tile_idx = ppu.sprite_ram[n*4+1];
		attrib = ppu.sprite_ram[n*4+2];
		x = ppu.sprite_ram[n*4+3];
		y = ppu.sprite_ram[n*4]+1;

		if(ppu.sprite_ram[n*4] == 239)
			ppu.sprite_overflow = 1;

		else if(ppu.sprite_ram[n*4] == 255)
			ppu.sprite_overflow = 0;

	
		range = ppu.scanline - y;
	
		if(range < 8 && size == 8){
			ppu.sprites_on_scanline++;
			if(ppu.sprites_on_scanline >= SPRITE_COUNT_OVERFLOW)
				ppu.sprite_overflow = 1;

			if( (attrib & SPRITE_BG_PRIO) == t) continue; 
		
			render_sprite_8_x_8(tile_idx,attrib,x,y,range);
		}
		else if(range < 16 && size == 16){
			ppu.sprites_on_scanline++;
			if(ppu.sprites_on_scanline >= SPRITE_COUNT_OVERFLOW)
				ppu.sprite_overflow = 1;

			if( (attrib & SPRITE_BG_PRIO) == t) continue; 
			
			render_sprite_8_x_16(tile_idx,attrib,x,y,range);
		}
	}

}

void render_sprite_8_x_16(__u8 tile_idx, __u8 attrib, __u8 x, __u8 y, __u8 range){
	__u16 ptrn_tbl_addr;
	__u8 n,m,k;
	__u8 pattern1, pattern2;
	__u8 palette_bits;

	if(range<8){
		if(tile_idx % 2 == 0)
			ptrn_tbl_addr = 0x0000;
		else {
			ptrn_tbl_addr = 0x1000;
			tile_idx -= 1;
		}
	}
	else {
		range+=8;
		if(tile_idx % 2 == 0){
			ptrn_tbl_addr = 0x0000;
			tile_idx += 1;
		}
		else {
			ptrn_tbl_addr = 0x1000;
			tile_idx -= 1;
		}
	}

	if(attrib & SPRITE_VFLIP)
		range ^= 0x07;
					
	pattern1 = ppu_vram_read(ptrn_tbl_addr + (tile_idx<<4) + range);
	pattern2 = ppu_vram_read(ptrn_tbl_addr + (tile_idx<<4) + range + 8);

	if(attrib & SPRITE_HFLIP){
		pattern1 = flip_bits(pattern1);
		pattern2 = flip_bits(pattern2);

	}
	
	palette_bits = ((attrib & 0x3) << 2);

	for(n=0;n<8;n++){
		k = palette_bits;
		k |= pattern1 & (0x80 >> n) ? 0x1 : 0;
		k |= pattern2 & (0x80 >> n) ? 0x2 : 0;

		if( (k & 0x3) != 0){
			// sprite0 hit test here
			setpixel(x+n,ppu.scanline,palette_table[ppu_vram_read(0x3F00 + k)]);
		}
	}
}

void render_sprite_8_x_8(__u8 tile_idx, __u8 attrib, __u8 x, __u8 y, __u8 range){
	__u8 n,m,k;
	__u8 pattern1, pattern2;
	__u8 palette_bits;

	if(attrib & SPRITE_VFLIP)
		range ^= 0x07;

	pattern1 = ppu_vram_read(ppu.sprite_ptrn_tbl_addr + (tile_idx<<4) + range);
	pattern2 = ppu_vram_read(ppu.sprite_ptrn_tbl_addr + (tile_idx<<4) + range + 8);

	if(attrib & SPRITE_HFLIP){
		pattern1 = flip_bits(pattern1);
		pattern2 = flip_bits(pattern2);

	}

	palette_bits = ((attrib & 0x3) << 2);
	
	for(n=0;n<8;n++){
		k = palette_bits;
		k |= pattern1 & (0x80 >> n) ? 0x1 : 0;
		k |= pattern2 & (0x80 >> n) ? 0x2 : 0;


		if( (k & 0x3) != 0){
			// sprite0 hit test here
			setpixel(x+n,ppu.scanline,palette_table[ppu_vram_read(0x3F00 + k)]);
		}
	}
}

__u8 flip_bits(__u8 k){
	__u8 r = 0;

	if(k & 0x80){ r |= 0x01; }
	if(k & 0x40){ r |= 0x02; }
	if(k & 0x20){ r |= 0x04; }
	if(k & 0x10){ r |= 0x08; }
	if(k & 0x08){ r |= 0x10; }
	if(k & 0x04){ r |= 0x20; }
	if(k & 0x02){ r |= 0x40; }
	if(k & 0x01){ r |= 0x80; }

	return r;
}

void render_bg(){
	__u16 tile_addr,attrib_table_addr,name_tbl_addr,tile_attrib_addr;

	__u8 tile_x_coord;
	__u8 tile_y_coord;
	__u8 tile_idx;
	__u8 tile_plane1;
	__u8 tile_plane2;
	__u8 tile_attrib_val;
	__u8 tile_attrib_idx;
	__u8 tile_attrib_up_bits;
	__u8 tile_palette_idx;

	__u8 tile_scroll_x, tile_scroll_y;

	__u8 n;
	__u16 t;
	__u8 pixel;
	_rgb p;

	for(n=0;n<33;n++){
		switch ((ppu.vram_addr >> 10) & 0x03){
			case 0: name_tbl_addr = 0x2000; break;
			case 1: name_tbl_addr = 0x2400; break;
			case 2: name_tbl_addr = 0x2800; break;
			case 3: name_tbl_addr = 0x2C00; break;
		}
	
		attrib_table_addr = name_tbl_addr + 0x03C0;
		tile_x_coord = (ppu.vram_addr & 0x1F);
		tile_scroll_x = ppu.x_scroll;

		tile_y_coord = ((ppu.vram_addr>>5) & 0x1F);
		tile_scroll_y = (ppu.vram_addr>>12) & 0x07;

		tile_addr = name_tbl_addr | (ppu.vram_addr & 0x03FF);

		for(pixel=0;pixel<8;pixel++){
			tile_idx = (ppu_vram_read(tile_addr));
		
			/* get pattern data */

			tile_plane1 = ppu_vram_read((tile_idx<<4) + tile_scroll_y + ppu.bg_pattern_tbl_addr);
			tile_plane2 = ppu_vram_read((tile_idx<<4) + tile_scroll_y + ppu.bg_pattern_tbl_addr + 8);

			/* get attrib data */
			tile_attrib_addr = attrib_table_addr | ( ((((tile_y_coord*8) + tile_scroll_y) / 32) * 8) + (((tile_x_coord*8) + tile_scroll_x) / 32) );
 			tile_attrib_val = ppu_vram_read(tile_attrib_addr);
			tile_attrib_idx = (((tile_x_coord % 4) & 0x2) >> 1) + ((tile_y_coord % 4) & 0x2)
;
			tile_attrib_up_bits = ((tile_attrib_val >> (tile_attrib_idx<<1)) & 0x3) << 2;

			tile_palette_idx = tile_attrib_up_bits;
			tile_palette_idx |= (tile_plane1 & (0x80 >> tile_scroll_x) ? 0x1 : 0);
			tile_palette_idx |= (tile_plane2 & (0x80 >> tile_scroll_x) ? 0x2 : 0);

			if((tile_palette_idx & 0x3) == 0){
				// transparent pixel
			}
			else {
				p.r = palette_table[ppu_vram_read(0x3F00 + tile_palette_idx)].r;
				p.g = palette_table[ppu_vram_read(0x3F00 + tile_palette_idx)].g;
				p.b = palette_table[ppu_vram_read(0x3F00 + tile_palette_idx)].b;
				setpixel((n<<3)+pixel,ppu.scanline,p);
				// render pixel to buffer				
			}
				
			tile_scroll_x++;
			
			if(tile_scroll_x >= 8){
				tile_scroll_x = 0;
				tile_addr++;
				tile_x_coord++;
			
				if ((tile_addr & 0x1F) == 0){
					tile_addr--;
					tile_x_coord--;
					tile_addr &= ~0x001F;
					tile_addr ^= 0x0400;
				}
			}
		}
		if ((ppu.vram_addr & 0x001F) == 31){
			ppu.vram_addr &= ~0x001F;
			ppu.vram_addr ^= 0x0400;
		}
		else {
			ppu.vram_addr++;		
		}
	}

	if ((ppu.vram_addr & 0x7000) != 0x7000){
		ppu.vram_addr += 0x1000;
	}
	else {
		ppu.vram_addr &= 0x0FFF;
		t = (ppu.vram_addr & 0x03E0) >> 5;

		if(t == 29){
			t = 0;
			ppu.vram_addr ^= 0x0800;
		}
		else if(t == 31)
			t = 0;
		else
			t++;

		ppu.vram_addr = (ppu.vram_addr & ~0x03E0) | (t << 5);
	}
}

void ppu_init(){
	ppu.vblank_active = 0;
	ppu.sprite0_hit = 0;
	ppu.sprite_overflow = 1;
}

void ppu_start(){
 	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
 	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &last);	
	ppu.vblank_nmi_done = 0;			
	ppu.scanline = 241;
	ppu.prev_scanline = 241;
	ppu.sprite0_hit = 0;
	ppu.sprite_overflow = 0;
	ppu.cycle_count = 0;
	ppu.vram_addr = 0;
	ppu.vram_addr_tmp = 0;

}

void ppu_stop(){
}

void ppu_reset(){
	ppu.scanline = 241;
}

__u16 handle_addr_mirror(__u16 addr){
	addr &= 0x3FFF;
		
	/* nametables  */
	addr = mirror_name_table(addr);
	
	/* nametables from 0x2000 to 0x2EFF are mirrored
 	   to 0x3000 to 3EFF */
	if(addr>= 0x2000 && addr <= 0x3EFF){
		addr &= ~0x1000;
	} /* palettes from 0x3F00 to 0x3F1F are mirrored 
	     to 0x3F20 to 0x3FFF */
	else if(addr >= 0x3F00 && addr <= 0x3FFF){
		addr &= ~0x00E0;
	}

	if( addr == 0x3F10 ||
	    addr == 0x3F14 || 
	    addr == 0x3F18 ||
	    addr == 0xF1C){
		addr &= ~0xF0;
	}
	return addr;
}

__u16 mirror_name_table(__u16 addr){
	if(mirroring == MIRROR_HORIZONTAL){	/* horizontal */
		if(addr >= 0x2400 && addr <= 0x27FF)
			addr -= 0x400;
		if(addr >= 0x2800 && addr <= 0x2BFF)
			addr -= 0x400;
		if(addr >= 0x2C00 && addr <= 0x2FFF)
			addr -= 0x800;
	}
	else if(mirroring == MIRROR_VERTICAL){ /* vertical */
		if(addr >= 0x2800 && addr <= 0x2FFF)
			addr -= 0x800;
	}
	else if(mirroring == MIRROR_SINGLE_SCREEN){ /* single screen */
		if(addr >= 0x2C00 && addr <= 0x2FFF)
			addr -= 0xC00;
		else if(addr >= 0x2800 && addr <= 0x2BFF)
			addr -= 0x800;
		else if(addr >= 0x2400 && addr <= 0x27FF)
			addr -= 0x400;
	}
	else if(mirroring == MIRROR_4SCREEN){ /* four screen */
		/* no remaps */
	}
	return addr;
}
