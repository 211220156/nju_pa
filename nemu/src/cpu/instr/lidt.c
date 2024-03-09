#include "cpu/instr.h"
/*
Put the implementations of `lidt' instructions here.
*/

make_instr_func(lidt)
{
	int len = 1;
	OPERAND opr;
	len += modrm_rm(eip + 1, &opr);
	opr.data_size = 16;
	operand_read(&opr);
	cpu.idtr.limit = opr.val;
	opr.data_size = 32;
	opr.addr += 2;
	operand_read(&opr);
	cpu.idtr.base = opr.val;
	return len;	
}

