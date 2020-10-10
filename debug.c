#include "debug.h"
#include "nes.h"

void debug_print(const char *fmt,...){
#ifdef DEBUG
		va_list args;
		va_start(args,fmt);
		vfprintf(stdout,fmt,args);
		va_end(args);
#endif
}


void debug_to_file(__u8 type, const char *fmt,...){
#ifdef ENABLE_DEBUG_CPU
	if(type = DEBUG_CPU){
		va_list args;
		va_start(args,fmt);
		vfprintf(d_cpu_fp,fmt,args);
		va_end(args);
	}
#endif
}

void open_debug_files(){
#ifdef ENABLE_DEBUG_CPU
		d_cpu_fp = fopen(DEBUG_CPU_FILE,"w");
#endif
}

void close_debug_files(){
#ifdef ENABLE_DEBUG_CPU
		fclose(d_cpu_fp);
#endif
}
