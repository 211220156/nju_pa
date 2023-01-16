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
make_instr_impl_1op(push, i, v)

make_instr_func(pusha) {
	uint32_t temp = cpu.esp;
	OPERAND opr;
	opr.data_size=32;
	opr.type=OPR_MEM;
	opr.sreg=SREG_DS;
	//push eax
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = cpu.eax;
	operand_write(&opr);
	//push ecx
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = cpu.ecx;
	operand_write(&opr);
	//push edx
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = cpu.edx;
	operand_write(&opr);
	//push ebx
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = cpu.ebx;
	operand_write(&opr);
	//push temp
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = temp;
	operand_write(&opr);
	//push ebp
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = cpu.ebp;
	operand_write(&opr);
	//push esi
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = cpu.esi;
	operand_write(&opr);
	//push edi
	cpu.esp -= data_size / 8;
	opr.addr = cpu.esp;
	opr.val = cpu.edi;
	operand_write(&opr);

	print_asm_0("pusha", "", 1);
	return 1;
	
}
