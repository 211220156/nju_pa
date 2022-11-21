#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
make_instr_func(lgdt){
    int len = 1;
    OPERAND opr;
    //先读取16位的limit
    opr.data_size = 16;
    len += modrm_rm(eip + 1, &opr);
    OPERAND_READ(&opr);
    cpu.gdtr.limit = opr.val;
    //继续读取32位的base，16位limit占两字节，因此opr的地址需要+2
    opr.addr += 2;
    opr.data_size = 32;
    OPERAND_READ(&opr);
    cpu.gdtr.base = opr.val;
    return len;
}