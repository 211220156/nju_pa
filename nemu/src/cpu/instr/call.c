#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near)
{
//    printf("callcallcall");
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip + 1;

    operand_read(&rel);//去eip+1处读取偏移量
    //push eip
    cpu.gpr[4].val -= (data_size / 8);
    OPERAND opr;
    opr.type = OPR_MEM;
    opr.data_size = data_size;
    opr.addr = cpu.gpr[4].val;
    opr.val = cpu.eip + 1 + data_size / 8;
    operand_write(&opr);
    
    int offset = sign_ext(rel.val, data_size);
    cpu.eip += offset;
    return 1 + data_size / 8;//???这个返回值什么用？
}