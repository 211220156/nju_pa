#include "cpu/instr.h"
/*
Put the implementations of `add' instructions here.
*/
static void instr_execute_2op()
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
    if (opr_src.type == OPR_IMM && opr_src.data_size < opr_dest.data_size){
        opr_src.val = sign_ext(opr_src.val, opr_src.data_size);
    }
    opr_dest.val = alu_add(opr_src.val, opr_dest.val, data_size);
    operand_write(&opr_dest);
}
make_instr_impl_2op(add, i, rm, v)
make_instr_impl_2op(add, i, a, v)
make_instr_impl_2op(add, r, rm, v)
make_instr_impl_2op(add, i, rm, bv)
make_instr_impl_2op(add, r, rm, b)