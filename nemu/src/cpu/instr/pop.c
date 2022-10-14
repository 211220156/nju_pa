#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    OPERAND top;
    top.data_size = data_size;
    top.type = OPR_MEM;
    top.addr = cpu.gpr[4].val;
    operand_read(&top);
    cpu.gpr[4].val += (top.data_size / 8); 
    opr_src.val = top.val;
    operand_write(&opr_src);
}
make_instr_impl_1op(pop, r, v)