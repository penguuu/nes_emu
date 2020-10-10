#define PPU_PER_CPU		3
#define CYCLES_PER_SCANLINE	341

#define VRAM_SIZE		16384
#define SPRITE_RAM_SIZE		256

#define SPRITE_COUNT_OVERFLOW 	8

#define SPRITE_VFLIP		0x80
#define SPRITE_HFLIP		0x40
#define SPRITE_BG_PRIO		0x20

typedef struct __ppu {
	__u8 enabled;	// is ppu enabled

	// 0x2000
	__u16 base_nametable_addr; // 0x2000, 0x2400, 0x2800, 0x2C00
	__u8 vram_addr_increment; // 1 = add 1 going across, 32 = add 32 going down
	__u16 sprite_ptrn_tbl_addr; // ignored for 8x16 mode
	__u16 bg_pattern_tbl_addr;
	__u8 sprite_size; // 8 = 8x8, 16 = 8x16
	__u8 ppu_master_slave_select; // (0 read backgdrop from ext pins, 1 = output color on ext pins
	__u8 nmi_on_vblank; // 0 = off, 1 = on
	__u8 vblank_nmi_done;

	// 0x2001
	__u8 intensify_blue;	/* these darken other colors */
	__u8 intensify_green;
	__u8 intensify_red;
	__u8 show_sprites;
	__u8 show_background;
	__u8 show_sprites_left_8; // 1 = show, 0 = hide
	__u8 show_bg_left_8; // 1 = show, 0 = hide
	__u8 grayscale; // 0 = normal, 1 = monochrome

	// 0x2003 and 0x2004
	__u8 oam_addr;

	// 0x2005 and 0x2006
	__u8 addr_latch; // used for 0x2005 and 0x2006 writes
	__u8 scroll_latch;
	__u16 vram_addr;
	__u16 vram_addr_tmp;
	__u16 x_scroll;
	__u16 y_scroll;
	
	__u8 vram[VRAM_SIZE];
	__u8 sprite_ram[SPRITE_RAM_SIZE];
	__u8 sprite_buf[32];

	__u8 sprites_on_scanline;

	__u16 cycle_count;
	__u16 prev_scanline;

	__u8 vblank_active;
	__u8 sprite0_hit;
	__u8 sprite_overflow;

	__u8 rendering; // 1 or 0 

	int scanline;
} _ppu;

_ppu ppu;

__u16 mirror_name_table(__u16);
__u16 handle_addr_mirror(__u16);
void ppu_reset();
void ppu_stop();
void ppu_start();
void ppu_init();
void render_bg();
void ppu_tick(__u16);
__u8 ppu_vram_read(__u16);
void ppu_vram_write(__u16,__u8);
void ppu_reg_write(__u16,__u8);
__u8 ppu_reg_read(__u16);
void render_sprites(__u8, __u8);
void render_sprite_8_x_16(__u8,__u8,__u8,__u8,__u8);
void render_sprite_8_x_8(__u8,__u8,__u8,__u8,__u8);
__u8 flip_bits(__u8);
struct timespec calc_diff(struct timespec *, struct timespec *);

typedef struct __rgb {
	__u8 r;
	__u8 g;
	__u8 b;
} _rgb;
//_rgb palette_table[2];

_rgb visible_area[240][256];
struct timespec now,last,diff;

