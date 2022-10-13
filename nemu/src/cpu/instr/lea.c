#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/
make_instr_func(lea)
{
    opr_dest.val = opr_src.addr;
    operand_write(&opr_dest);
    return 1 + opr_dest.data_size / 8 + opr_src.data_size / 8;
}