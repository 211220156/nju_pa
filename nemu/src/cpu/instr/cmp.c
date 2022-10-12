#include "cpu/instr.h"
/*
Put the implementations of `cmp' instructions here.
*/
static void instr_execute_2op()
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
	alu_sub(sign_ext(opr_src.val, 8), opr_dest.val, 8);
}

make_instr_impl_2op(cmp, i, rm, b);