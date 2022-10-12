#include "cpu/instr.h"
/*
Put the implementations of `adc' instructions here.
*/
static void instr_execute_2op() 
{
	operand_read(&opr_src);
	opr_dest.val = opr_dest.val + opr_src.val + cpu.eflags.CF;
	operand_write(&opr_dest);
}
