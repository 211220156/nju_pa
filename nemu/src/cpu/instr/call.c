#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near)
{
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip + 1;

    operand_read(&rel);
    //push eip
    cpu.gpr[4].val -= (data.size / 8);
    OPERAND opr;
    opr.type = OPR_MEM;
    opr.addr = cpu.gpr[4].val;
    opr.val = cpu.eip;
    operand_write(&opr);
    
    int offset = sign_ext(rel.val, data_size);
    cpu.eip += offset;
    
    return 0;
}