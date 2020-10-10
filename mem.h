#include <asm/types.h>

__u8 read_mem(__u16);
void write_mem(__u16, __u8);

__u8 ram[0xFFFF];
