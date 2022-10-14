#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
/*    cpu.gpr[4].val -= (data_size / 8);
    if (opr_src.type == OPR_IMM) {
        opr_src.val = sign_ext(opr_src.val, opr_src.data_size);
    }*/
    OPERAND opr;
    cpu.gpr[4].val -= (data_size / 8);
    opr.type = OPR_MEM;
    opr.addr = cpu.gpr[4].val;
    opr.data_size = data_size;
    opr.val = opr_src.val;
    opr.sreg = SREG_CS;
    operand_write(&opr);
}

make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, rm, v)
make_instr_impl_1op(push, i, b)