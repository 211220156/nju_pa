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
make_instr_func(ret_near_imm16)
{
    
    OPERAND opr;
    opr.type = OPR_MEM;
    opr.data_size = data_size;
    opr.addr = cpu.gpr[4].val;
    operand_read(&opr);
    cpu.gpr[4].val += (data_size / 8);//这里修改了esp的值
    
    //先把eip+1后的立即数取出来加到esp上。
    OPERAND imm16;
    imm16.type = OPR_IMM;
    imm16.data_size = 16;
    imm16.sreg = SREG_CS;
    imm16.addr = cpu.eip + 1;
    operand_read(&imm16);
    cpu.gpr[4].val += imm16.val;
    
    cpu.eip = opr.val;
/*    if (data_size == 16){
        cpu.eip &= 0x0000ffff;
    }
    */
    return 0;
}