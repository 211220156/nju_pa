#include "cpu/intr.h"
#include "cpu/instr.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/
make_instr_func(int_)
{
	OPERAND opr;
    	opr.type = OPR_IMM;
	opr.sreg = SREG_CS;
    	opr.data_size = 8;
    	opr.addr = eip + 1;
    	operand_read(&opr);
	print_asm_1("int", "", 2, &opr);
	raise_sw_intr(opr.val);
	return 0;
}
