#include "nes.h"
#include <stdarg.h>

#define ENABLE_DEBUG_CPU 0
#define DEBUG_CPU	0x01

#define DEBUG		0
#define DEBUG_CPU_FILE	"debug_cpu.txt"

void debug_print(const char *fmt,...);
void debug_to_file(__u8 type,const char *fmt,...);
void open_debug_files();
void close_debug_files();

FILE *d_cpu_fp;
