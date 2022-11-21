#include "cpu/instr.h"

make_instr_func(jmp_near)
{
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip + 1;

    operand_read(&rel);

    int offset = sign_ext(rel.val, data_size);
    // thank Ting Xu from CS'17 for finding this bug
    print_asm_1("jmp", "", 1 + data_size / 8, &rel);

    cpu.eip += offset;

    return 1 + data_size / 8;
}
make_instr_func(jmp_short)
{
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;

    operand_read(&rel);

    int offset = sign_ext(rel.val, rel.data_size);

    cpu.eip += offset;

    return 1 + 8 / 8;
}
make_instr_func(jmp_near_indirect)
{
    OPERAND offset;
    offset.data_size = data_size;
    offset.sreg = SREG_CS;
    modrm_rm(cpu.eip + 1, &offset);
    
    operand_read(&offset);
    
    cpu.eip = offset.val;
    if (offset.data_size == 16){
        cpu.eip = cpu.eip & 0x0000ffff;
    }
    
    return 0;
}
//ljmp
make_instr_func(jmp_far_imm){
    OPERAND opr;
    opr.data_size = 32;
    opr.addr = eip + 1;
    opr.sreg = SREG_CS;
    opr.type = OPR_IMM;
    operand_read(&opr);
    
    if (data_size == 16){
        cpu.eip = opr.val & 0xffff;
    } else {
        cpu.eip = opr.val;
    }
    //装载段寄存器可见部分
    cpu.cs.val = instr_fetch(eip + 5, 2);
    
    load_sreg(1);//装载code segment寄存器隐藏部分
    
    return 0;
}