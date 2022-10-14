#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near)
{/*
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
    return 1 + data_size / 8;//???这个返回值什么用？*/

    //与jmp类似，跳转前先将返回地址push进栈
    cpu.gpr[4].val -= 4;

    //将栈顶地址准备好
    OPERAND top;
    top.type = OPR_MEM;
    top.addr = cpu.gpr[4].val;
    top.data_size = 32;
    top.sreg = SREG_CS;

    //将eip值（返回地址）写进栈顶地址（入栈）
    top.val = cpu.eip + 1 + data_size / 8;
    operand_write(&top);

    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = cpu.eip + 1;
    //the value of rel
    operand_read(&rel);
    
    int offset = sign_ext(rel.val, data_size);

    cpu.eip += offset;

    return 1 + data_size / 8;
   
}