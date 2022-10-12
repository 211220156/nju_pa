#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret_near){
//    printf("retertret");
    OPERAND opr;
    opr.type = OPR_MEM;
    opr.data_size = data_size;
    opr.addr = cpu.gpr[4].val;
    operand_read(&opr);
    cpu.gpr[4].val += (data_size / 8);
    
    cpu.eip = opr.val;
    if (data_size == 16){
        cpu.eip &= 0x0000ffff;
    }
    
    return 0;
}