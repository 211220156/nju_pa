#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
make_instr_func(leave)
{
    cpu.esp = cpu.ebp;
    OPERAND top;
    top.data_size = data_size;
    top.type = OPR_MEM;
    top.addr = cpu.gpr[4].val;
    operand_read(&top);
    cpu.esp += (top.data_size / 8); 
    cpu.ebp = top.val;
}