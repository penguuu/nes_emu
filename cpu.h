#include <asm/types.h>

/* define registers */
#define BIT_CARRY_FLAG 		0x01
#define BIT_ZERO_FLAG 		0x02
#define BIT_INTERRUPT_FLAG 	0x04
#define BIT_DECIMAL_FLAG 	0x08	// unsupported on NES
#define BIT_BREAK_FLAG		0x10
#define BIT_CONSTANT_FLAG	0x20
#define BIT_OVERFLOW_FLAG 	0x40
#define BIT_NEGATIVE_FLAG 	0x80

/* stack location at memory */
#define STACK_ADDR		0x100

/* define addressing modes */
#define IMPLIED            0x1
#define ACCUMULATOR        0x2
#define IMMEDIATE          0x4
#define ZERO_PAGE          0x8
#define ZERO_PAGE_X       0x10
#define ZERO_PAGE_Y       0x20
#define INDIRECT_X        0x40
#define INDIRECT_Y        0x80
#define RELATIVE         0x100
#define ABSOLUTE         0x200
#define ABSOLUTE_X       0x400
#define ABSOLUTE_Y       0x800
#define INDIRECT        0x1000

#define READ_MEM	0x01
#define NO_READ		0x00

/* registers */
__u8 A,X,Y,S,SP;
__u16 PC;
__u16 addr_mode;
__u64 ticks;

__u8 cross_page_penalty;
__u16 p_addr;

/* flag setting */
#define set_carry() S = S | BIT_CARRY_FLAG
#define clear_carry() S = S & (~BIT_CARRY_FLAG)

#define set_zero() S = S | BIT_ZERO_FLAG
#define clear_zero() S = S & (~BIT_ZERO_FLAG)

#define set_interrupt() S = S | BIT_INTERRUPT_FLAG
#define clear_interrupt() S = S & (~BIT_INTERRUPT_FLAG)

#define set_overflow() S = S | BIT_OVERFLOW_FLAG
#define clear_overflow() S = S & (~BIT_OVERFLOW_FLAG)

#define set_negative() S = S | BIT_NEGATIVE_FLAG
#define clear_negative() S = S & (~BIT_NEGATIVE_FLAG)

#define set_decimal() S = S | BIT_DECIMAL_FLAG
#define clear_decimal() S = S & (~BIT_DECIMAL_FLAG)

#define set_constant() S = S | BIT_CONSTANT_FLAG
#define clear_constant() S = S & (~BIT_CONSTANT_FLAG)

#define set_break() S = S | BIT_BREAK_FLAG
#define clear_break() S = S & (~BIT_BREAK_FLAG)

#define carry_test(k) { if((k) & 0xFF00) set_carry(); else clear_carry(); }
#define zero_test(k) {  if((k) & 0x00FF) clear_zero(); else set_zero(); }
#define sign_test(k) {  if(((k) & 0x0080) != 0) set_negative(); else clear_negative(); }
#define overflow_test(k,a_r,t) { if( ((k) ^ (__u16)a_r) & ((k) ^ (t)) & 0x0080) set_overflow(); else clear_overflow(); }

/* macros to get 8 high/low bits of 16-bit value */
#define high_bits(k) (((k) & 0xFF00)>>8)
#define low_bits(k) ((k) & 0x00FF)

#define SET_A(k) A = (__u8)( (k) & 0x00FF)

#define zero_page_wrap(k) ((k) & 0xFF)
#define page_address(k) ((k) & 0xFF00)

#define flag_set(f) ((S & f) == f)
#define flag_unset(f) ((S & f) == 0)

#define load_reset_vector() (PC = read_mem(0xFFFC) | (read_mem(0xFFFD) << 8))
#define load_nmi_vector() (PC = read_mem(0xFFFA) | (read_mem(0xFFFB)<<8))
#define load_irq_vector() (PC = read_mem(0xFFFE) | (read_mem(0xFFFF)<<8))

/* for debugging */
__u8 dt1, dt2, dret;
__u8 d_A,d_X,d_Y,d_S,d_SP;
__u16 d_PC,d_loc,d_loc2;
__u8 nmi_in_queue;

__u8 pop_stack8();
void push_stack8(__u8);
__u16 pop_stack16();
void push_stack16(__u16);
