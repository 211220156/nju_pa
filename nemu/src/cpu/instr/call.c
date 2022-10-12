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
    cpu.gpr[4].val -= (data_size / 8);
    OPERAND opr;
    opr.type = OPR_MEM;
    opr.data_size = data_size;
    opr.addr = cpu.gpr[4].val;
    opr.val = cpu.eip;
    operand_write(&opr);
    
    if (rel.data_size == 16){
        cpu.eip = (cpu.eip + sign_ext(rel.val, data_size)) & 0x0000ffff;
    } else {
        cpu.eip = cpu.eip + rel.val;
    }
//    int offset = sign_ext(rel.val, data_size);
//    cpu.eip += offset;
    cpu.eip += 5;
    return 0;
}