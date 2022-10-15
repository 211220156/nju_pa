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