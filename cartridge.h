#include <asm/types.h>

#define FILENAME_LEN 512

__u8 header[16];
__u8 trainer[512];
__u8 title[128];
__u8 *prg_rom;
__u8 *chr_rom;
__u8 prg_units;
__u8 chr_units;
