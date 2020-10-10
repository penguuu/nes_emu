#include "cpu.h"
#include "nes.h"
#include "memory.h"
#include "opcode_table.h"

__u8 pop_stack8(){
	__u8 ret;
	ret = read_mem(STACK_ADDR + 1 + SP);
	SP++;
	return ret;
}

void push_stack8(__u8 byte){
	write_mem(STACK_ADDR + SP,byte);
	SP--;
}
__u16 pop_stack16(){
	__u16 tmp_val;
	tmp_val = read_mem(STACK_ADDR + 1 + SP);
	SP++;
	tmp_val += ((read_mem(STACK_ADDR + 1 + SP)<<8) & 0xFF00);
	SP++;
	return tmp_val;
}
void push_stack16(__u16 byte){
	write_mem(STACK_ADDR + SP,high_bits(byte));
	SP--;
	write_mem(STACK_ADDR + SP,low_bits(byte));
	SP--;
}

__u8 read_param(__u8 read_flag){
	__u8 ret = 0;
	__u8 t1,t2,t3;
	__u16 addr,addr2,addr3;

	if(addr_mode & IMPLIED){
		return 0;
	}
	else if(addr_mode & ACCUMULATOR){
		ret = A;
	}
	else if(addr_mode & IMMEDIATE){
		p_addr = PC;
		if(read_flag == READ_MEM)			
			ret = read_mem(p_addr);		
		PC++;
	}
	else if(addr_mode & ZERO_PAGE){
		t1 = read_mem(PC);
		p_addr = t1;
		if(read_flag == READ_MEM)
			ret = read_mem(p_addr);
		PC++;
	}
	else if(addr_mode & ZERO_PAGE_X){
		t1 = read_mem(PC);
		p_addr = low_bits(t1+X);
		if(read_flag == READ_MEM)
			ret = read_mem(p_addr);
		PC++;
	}
	else if(addr_mode & ZERO_PAGE_Y){
		t1 = read_mem(PC);
		p_addr = low_bits(t1+Y);
		if(read_flag == READ_MEM)
			ret = read_mem(p_addr);
		PC++;
	}
	else if(addr_mode & ABSOLUTE){
		t1 = read_mem(PC);
		PC++;
		t2 = read_mem(PC);
		PC++;
		p_addr = ((t2<<8)+t1);
		if(read_flag == READ_MEM)
			ret = read_mem(p_addr);
	}
	else if(addr_mode & ABSOLUTE_X){
		t1 = read_mem(PC);
		PC++;
		t2 = read_mem(PC);
		PC++;
		addr = page_address((t2<<8)+t1);
		if( page_address(( (t2<<8) +t1 ) + X ) != addr)
			cross_page_penalty = 1;

		p_addr = ((t2<<8)+t1)+X;
		d_loc = p_addr;
		if(read_flag == READ_MEM)
			ret = read_mem(p_addr);
	}
	else if(addr_mode & ABSOLUTE_Y){
		t1 = read_mem(PC);
		PC++;
		t2 = read_mem(PC);
		PC++;
		addr = page_address((t2<<8)+t1);
		if( page_address(( (t2<<8) +t1 ) + Y ) != addr)
			cross_page_penalty = 1;
		
		p_addr = ((t2<<8)+t1)+Y;
		d_loc = p_addr;
		if(read_flag == READ_MEM)
	                ret = read_mem(p_addr);
	}
	else if(addr_mode & INDIRECT_X){
		t1 = read_mem(PC);
		PC++;
		addr = zero_page_wrap(t1+X);
		addr2 = (read_mem(addr & 0x00FF)) | (read_mem((addr+1)&0x00FF)<<8);
		p_addr = addr2;
		d_loc = p_addr;
		if(read_flag == READ_MEM)
			ret = read_mem(p_addr);
	}
	else if(addr_mode & INDIRECT_Y){
		t1 = (read_mem(PC)&0xFF);
		PC++;
		t2 = read_mem((t1+1) & 0xFF);
		t3 = read_mem(t1 & 0xFF);
		addr = ((t2<<8)|t3)+Y;
		p_addr = addr;
		d_loc = p_addr;
		d_loc2 = ((t2<<8)|t3);
		/* cross page penalty calc here */
		if(read_flag == READ_MEM)
			ret = read_mem(p_addr);
	}
	dt1 = t1;
	dt2 = t2;
	dret = ret;
	return ret;
}

/* returns the address for indirect commands */
__u16 read_indirect_param(){
	__u16 ret,addr,addr2;
	__u8 t1,t2;
	if(addr_mode & INDIRECT){
		t1 = read_mem(PC);
		PC++;
		t2 = read_mem(PC);
		PC++;

		addr = (t2<<8) | t1;
		addr2 = (addr & 0xFF00) | ((addr+1) & 0x00FF);
		
		dt1 = t1;
		dt2 = t2;
		ret = read_mem(addr) | (read_mem(addr2)<<8);
		d_loc = ret;
		
	}
	return ret;
}

void handle_relative(__u8 condition_true){
	__u16 ret;
	__u16 old_pc;

	old_pc = PC;
	ret = read_mem(PC);
	PC++;

	if(ret & 0x80) ret |= 0xFF00;

	dt1 = low_bits(ret);
	d_loc = PC+ret;

	if(condition_true == 1){
		PC=PC+ret;


		if ( page_address(old_pc) != page_address(PC) )
			cross_page_penalty = 2;
		else 
			cross_page_penalty = 1;
	}
}

void adc(){
	__u8 val;
	__u16 res;

	val = read_param(READ_MEM);

	res = A+val+(S&BIT_CARRY_FLAG);

	carry_test(res);
	zero_test(res);
	overflow_test(res,A,val);
	sign_test(res);

	SET_A(res);
}

void and(){
	__u8 val;
	__u8 res;

	val = read_param(READ_MEM);

	res = A & val;

	zero_test(res);
	sign_test(res);
	SET_A(res);	
}

void asl(){
	__u8 val;
	__u16 res; 

	val = read_param(READ_MEM);

	res = val << 1;

	carry_test(res);
	zero_test(res);
	sign_test(res);

	if(addr_mode == ACCUMULATOR)
		SET_A(res);
	else
		write_mem(p_addr,low_bits(res));		
}

void bcc(){ handle_relative(flag_unset(BIT_CARRY_FLAG)); }
void bcs(){ handle_relative(flag_set(BIT_CARRY_FLAG)); }
void beq(){ handle_relative(flag_set(BIT_ZERO_FLAG)); }

void bit(){
	__u16 res;
	__u8 val;

	val = read_param(READ_MEM);
	res = A & val;

	zero_test(res);
	S = ( S&0x3F) | (val & 0xC0);
}

void bmi(){ handle_relative(flag_set(BIT_NEGATIVE_FLAG)); }
void bne(){ handle_relative(flag_unset(BIT_ZERO_FLAG)); }
void bpl(){ handle_relative(flag_unset(BIT_NEGATIVE_FLAG)); }

void brk(){
	push_stack16(PC);
	push_stack16(S);
	set_break();
	load_irq_vector();
}

void bvc(){ handle_relative(flag_unset(BIT_OVERFLOW_FLAG)); }
void bvs(){ handle_relative(flag_set(BIT_OVERFLOW_FLAG)); } 

void clc(){ clear_carry(); }
void cld(){ clear_decimal(); }
void cli(){ clear_interrupt(); }
void clv(){ clear_overflow(); }

void cmp(){
	__u16 res;
	__u8 val;

	val = read_param(READ_MEM);
	
	res = A-val;

	if(A>=low_bits(val))
		set_carry();
	else  clear_carry();

	if(A==low_bits(val))
		set_zero();
	else clear_zero();

	sign_test(res);
}

void cpx(){
	__u16 res;
	__u8 val;

	val = read_param(READ_MEM);

	res = X-val;

	if(X>=low_bits(val))
		set_carry();
	else clear_carry();

	if(X==low_bits(val))
		set_zero();
	else clear_zero();

	sign_test(res);
}

void cpy(){
	__u16 res;
	__u16 val;

	val = read_param(READ_MEM);

	res = (__u16)Y-val;

	if(Y>=(__u8)low_bits(val))
		set_carry();
	else clear_carry();

	if(Y==(__u8)low_bits(val))
		set_zero();
	else clear_zero();

	sign_test(res);
}

void dec(){
	__u16 res;
	__u8 val;

	val = read_param(READ_MEM);
	res = val-1;

	zero_test(res);
	sign_test(res);

	write_mem(p_addr,low_bits(res));		
}

void dex(){
	X--;
	zero_test(X);
	sign_test(X);
}

void dey(){
	Y--;
	zero_test(Y);
	sign_test(Y);
}

void eor(){
	__u16 res;
	__u8 val;

	val = read_param(READ_MEM);
	res = A^val;

	zero_test(res);
	sign_test(res);

	SET_A(res);
}

void inc(){
	__u16 res;
	__u8 val;

	val = read_param(READ_MEM);
	res = val+1;

	zero_test(res);
	sign_test(res);

	write_mem(p_addr,low_bits(res));
}

void inx(){
	X++;
	zero_test(X);
	sign_test(X);
}

void iny(){
	Y++;
	zero_test(Y);
	sign_test(Y);
}

void jmp(){
	__u16 res;

	if(addr_mode & INDIRECT)
		res = read_indirect_param();
	else { 
		read_param(READ_MEM);
		res = p_addr;
	}

	PC=res;
}

void jsr(){
	__u16 res;

	read_param(READ_MEM);
	res = p_addr;
	push_stack16(PC-1);
	PC = res;
}

void lda(){
	__u8 val;

	val = read_param(READ_MEM);

	zero_test(val);
	sign_test(val);

	SET_A(val);
}

void ldx(){
	__u8 val;

	val = read_param(READ_MEM);

	zero_test(val);
	sign_test(val);

	X = val;
}

void ldy(){
	__u8 val;

	val = read_param(READ_MEM);

	zero_test(val);
	sign_test(val);

	Y = val;
}

void lsr(){
	__u8 val;
	__u16 res; 

	val = read_param(READ_MEM);

	res = val >> 1;
	
	if(val &1) set_carry();
	else clear_carry();

	zero_test(res);
	sign_test(res);

	if(addr_mode == ACCUMULATOR)
		SET_A(res);
	else
		write_mem(p_addr,low_bits(res));		
}

void nop(){}

void ora(){
	__u8 val;

	val = read_param(READ_MEM);

	A |= val;

	zero_test(A);
	sign_test(A);
}

void pha(){ push_stack8(A); }
void php(){ push_stack8(S); }
void pla(){ 
	A = pop_stack8(); 
	zero_test(A);
	sign_test(A);
}
void plp(){ 
	S = pop_stack8();
	S |= BIT_CONSTANT_FLAG;
	if(S&BIT_BREAK_FLAG) 
		S ^= BIT_BREAK_FLAG;
}

void rol(){
        __u8 val;
        __u16 res;

        val = read_param(READ_MEM);

        res = (val << 1) | (S & BIT_CARRY_FLAG);

        carry_test(res);
        zero_test(res);
        sign_test(res);

        if(addr_mode == ACCUMULATOR)
		SET_A(res);
        else
                write_mem(p_addr,low_bits(res));		
}

void ror(){
	__u8 val;
	__u16 res;

	val = read_param(READ_MEM);

	res = (val >> 1) | ((S & BIT_CARRY_FLAG) << 7);

	if(val & 1)
		set_carry();
	else
		clear_carry();

	zero_test(res);
	sign_test(res);

	if(addr_mode == ACCUMULATOR)
		SET_A(res);
	else
		write_mem(p_addr,low_bits(res));
}

void rti(){
	S = pop_stack8()|BIT_CONSTANT_FLAG;
	PC = pop_stack16();
}

void rts(){
	PC = pop_stack16();
	PC++;
}

void sbc(){
	__u8 val;
	__u16 res;

        val = read_param(READ_MEM)^0x00FF;

        res = (__u16)A+val+(__u16)(S&BIT_CARRY_FLAG);
        carry_test(res);
        zero_test(res);
        overflow_test(res,A,val);
        sign_test(res);

	SET_A(res);
}

void sec(){ set_carry(); }
void sed(){ set_decimal(); }
void sei(){ set_interrupt(); }

void sta(){ read_param(NO_READ); write_mem(p_addr,A); }
void stx(){ read_param(NO_READ); write_mem(p_addr,X); }
void sty(){ read_param(NO_READ); write_mem(p_addr,Y); }
void tax(){ X = A; zero_test(X); sign_test(X); }
void tay(){ Y = A; zero_test(Y); sign_test(Y); }
void tsx(){ X = SP; zero_test(X); sign_test(X); }
void txa(){ A=X; zero_test(A); sign_test(A); }
void txs(){ SP=X; }
void tya(){ A=Y; zero_test(A); sign_test(A); }

void cpu_start(){
	unsigned int n,l;
	
	debug_print("cpu_start()\n");
	debug_print("loading opcodes to lookup-table\n");
	for(n=0;n<(sizeof(opcodes)/sizeof(_opcodes));n++){
		l = opcodes[n].num;
		lookup_table[l] = opcodes[n];
	}
	debug_print("resetting vectors\n");
	load_reset_vector();
	debug_print("PC = %02X\n",PC);
	nmi_in_queue = 0;
	A = 0;
	X = 0;
	Y = 0;
	SP = 0xFD;
	S = 0x24;
//	PC=0xC000;
}

void cpu_reset(){
	load_reset_vector();
	SP = SP-3;
	S |= 0x04;
	// silence APU
}

void handle_opcode(__u8 op){
	#ifdef DEBUG
	d_A = A;
	d_X = X;
	d_Y = Y;
	d_S = S;
	d_SP = SP;
	d_PC = PC-1;
	#endif
	addr_mode = lookup_table[op].addr_mode;
	if(lookup_table[op].name_idx == 0) adc();
	else if(lookup_table[op].name_idx == 1) and();
	else if(lookup_table[op].name_idx == 2) asl();
	else if(lookup_table[op].name_idx == 3) bcc();
	else if(lookup_table[op].name_idx == 4) bcs();
	else if(lookup_table[op].name_idx == 5) beq();
	else if(lookup_table[op].name_idx == 6) bit();
	else if(lookup_table[op].name_idx == 7) bmi();
	else if(lookup_table[op].name_idx == 8) bne();
	else if(lookup_table[op].name_idx == 9) bpl();
	else if(lookup_table[op].name_idx == 10) brk();
	else if(lookup_table[op].name_idx == 11) bvc();
	else if(lookup_table[op].name_idx == 12) bvs();
	else if(lookup_table[op].name_idx == 13) clc();
	else if(lookup_table[op].name_idx == 14) cld();
	else if(lookup_table[op].name_idx == 15) cli();
	else if(lookup_table[op].name_idx == 16) clv();
	else if(lookup_table[op].name_idx == 17) cmp();
	else if(lookup_table[op].name_idx == 18) cpx();
	else if(lookup_table[op].name_idx == 19) cpy();
	else if(lookup_table[op].name_idx == 20) dec();
	else if(lookup_table[op].name_idx == 21) dex();
	else if(lookup_table[op].name_idx == 22) dey();
	else if(lookup_table[op].name_idx == 23) eor();
	else if(lookup_table[op].name_idx == 24) inc();
	else if(lookup_table[op].name_idx == 25) inx();
	else if(lookup_table[op].name_idx == 26) iny();
	else if(lookup_table[op].name_idx == 27) jmp();
	else if(lookup_table[op].name_idx == 28) jsr();
	else if(lookup_table[op].name_idx == 29) lda();
	else if(lookup_table[op].name_idx == 30) ldx();
	else if(lookup_table[op].name_idx == 31) ldy();
	else if(lookup_table[op].name_idx == 32) lsr();
	else if(lookup_table[op].name_idx == 33) nop();
	else if(lookup_table[op].name_idx == 34) ora();
	else if(lookup_table[op].name_idx == 35) pha();
	else if(lookup_table[op].name_idx == 36) php();
	else if(lookup_table[op].name_idx == 37) pla();
	else if(lookup_table[op].name_idx == 38) plp();
	else if(lookup_table[op].name_idx == 39) rol();
	else if(lookup_table[op].name_idx == 40) ror();
	else if(lookup_table[op].name_idx == 41) rti();
	else if(lookup_table[op].name_idx == 42) rts();
	else if(lookup_table[op].name_idx == 43) sbc();
	else if(lookup_table[op].name_idx == 44) sec();
	else if(lookup_table[op].name_idx == 45) sed();
	else if(lookup_table[op].name_idx == 46) sei();
	else if(lookup_table[op].name_idx == 47) sta();
	else if(lookup_table[op].name_idx == 48) stx();
	else if(lookup_table[op].name_idx == 49) sty();
	else if(lookup_table[op].name_idx == 50) tax();
	else if(lookup_table[op].name_idx == 51) tay();
	else if(lookup_table[op].name_idx == 52) tsx();
	else if(lookup_table[op].name_idx == 53) txa();
	else if(lookup_table[op].name_idx == 54) txs();
	else if(lookup_table[op].name_idx == 55) tya();

	#ifdef DEBUG
	debug_to_file(DEBUG_CPU,"%04X  %02X ",d_PC,op);
	if(lookup_table[op].addr_mode & IMPLIED)
		debug_to_file(DEBUG_CPU,"        %s                             ",opcode_names[lookup_table[op].name_idx]);
	else if(lookup_table[op].addr_mode & ACCUMULATOR)
		debug_to_file(DEBUG_CPU,"        %s A                           ",opcode_names[lookup_table[op].name_idx]);
	else if(lookup_table[op].addr_mode & IMMEDIATE)
		debug_to_file(DEBUG_CPU,"%02X      %s #$%02X                        ",dret,opcode_names[lookup_table[op].name_idx],dret);
	else if(lookup_table[op].addr_mode & ZERO_PAGE && (lookup_table[op].name_idx < 47 || lookup_table[op].name_idx > 49))
		debug_to_file(DEBUG_CPU,"%02X      %s $%02X = %02X                    ",dt1,opcode_names[lookup_table[op].name_idx],dt1,dret);
	else if(lookup_table[op].addr_mode & ZERO_PAGE){ // sta(), stx(), sty()	
		if(lookup_table[op].name_idx == 47) dt2 = A;
		else if(lookup_table[op].name_idx == 48) dt2 = X;
		else if(lookup_table[op].name_idx == 49) dt2 = Y;
		debug_to_file(DEBUG_CPU,"%02X      %s $%02X = %02X                    ",dt1,opcode_names[lookup_table[op].name_idx],dt1,dt2);
	}
	else if(lookup_table[op].addr_mode & ZERO_PAGE_X)
		debug_to_file(DEBUG_CPU,"%02X      %s $%02X,X = %02X                  ",dt1,opcode_names[lookup_table[op].name_idx],dt1,dret);
	else if(lookup_table[op].addr_mode & ZERO_PAGE_Y)
		debug_to_file(DEBUG_CPU,"%02X      %s $%02X,Y = %02X                  ",dt1,opcode_names[lookup_table[op].name_idx],dt1,dret);
	else if(lookup_table[op].addr_mode & INDIRECT_X)
		debug_to_file(DEBUG_CPU,"%02X      %s ($%02X,X) @ %02X = %04X = %02X    ",dt1,opcode_names[lookup_table[op].name_idx],dt1,dt1+d_X,d_loc,dret);
	else if(lookup_table[op].addr_mode & INDIRECT_Y)
		debug_to_file(DEBUG_CPU,"%02X      %s ($%02X),Y = %04X @ %04X = %02X  ",dt1,opcode_names[lookup_table[op].name_idx],dt1,d_loc2,d_loc,dret);
	else if(lookup_table[op].addr_mode & RELATIVE)
		debug_to_file(DEBUG_CPU,"%02X      %s $%04X                       ",dt1,opcode_names[lookup_table[op].name_idx],d_loc);
	else if(lookup_table[op].addr_mode & ABSOLUTE)
		debug_to_file(DEBUG_CPU,"%02X %02X   %s $%02X%02X = %02X                  ",dt1,dt2,opcode_names[lookup_table[op].name_idx],dt2,dt1,dret);
	else if(lookup_table[op].addr_mode & ABSOLUTE_X)
		debug_to_file(DEBUG_CPU,"%02X %02X   %s $%02X%02X,X @ %04X = %02X         ",dt1,dt2,opcode_names[lookup_table[op].name_idx],dt2,dt1,d_loc,dret);
	else if(lookup_table[op].addr_mode & ABSOLUTE_Y)
		debug_to_file(DEBUG_CPU,"%02X %02X   %s $%02X%02X,Y @ %04X = %02X         ",dt1,dt2,opcode_names[lookup_table[op].name_idx],dt2,dt1,d_loc,dret);
	else if(lookup_table[op].addr_mode & INDIRECT)
		debug_to_file(DEBUG_CPU,"%02X %02X   %s ($%02X%02X) = %04X              ",dt1,dt2,opcode_names[lookup_table[op].name_idx],dt2,dt1,d_loc);
	debug_to_file(DEBUG_CPU," A = %02X, X = %02X, Y = %02X, S = %02X, SP = %02X ",d_A,d_X,d_Y,d_S,d_SP);
	#endif
}

void nmi(){
	push_stack16(PC);
	push_stack8(S);
	load_nmi_vector();
}

void irq(){
	push_stack16(PC);
	push_stack8(S);
	load_irq_vector();
}

__u16 cpu_tick(){
	__u8 op;
	__u16 ticks_run = 0;
	op = read_mem(PC);
	PC++;

	cross_page_penalty = 0;
	handle_opcode(op);

	ticks_run += lookup_table[op].cycles;	

	if(cross_page_penalty &&
	 lookup_table[op].cross_page)
		ticks_run+=cross_page_penalty;

	return ticks_run;
}

__u16 cpu_run_ticks(__u16 amount){
	__u16 ticks_run = 0;
	__u16 count = 0;

	while(count < amount){
		ticks_run = cpu_tick();
		count += ticks_run;
		if(nmi_in_queue>0){
			nmi();
			nmi_in_queue--;
			count += 7;
		}
	}
	return(count);
}

