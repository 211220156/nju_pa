#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    OPERAND top;
    top.data_size = data_size;
    top.sreg = SREG_DS;
    top.type = OPR_MEM;
    top.addr = cpu.gpr[4].val;
    operand_read(&top);
    cpu.gpr[4].val += (top.data_size / 8); 
    opr_src.val = top.val;
    operand_write(&opr_src);
}
make_instr_impl_1op(pop, r, v)

make_instr_func(popa)
{
	OPERAND opr;
	opr.data_size = 32;
	opr.type = OPR_MEM;
	opr.sreg = SREG_DS;
	//pop edi
	opr.addr = cpu.esp;
	operand_read(&opr);
	cpu.edi = opr.val;
	cpu.esp += 32 / 8;
	//pop esi
	opr.addr = cpu.esp;
	operand_read(&opr);
	cpu.esi = opr.val;
	cpu.esp += 32 / 8;
	//pop ebp
	opr.addr = cpu.esp;
	operand_read(&opr);
	cpu.ebp = opr.val;
	cpu.esp += 32 / 8;
	//throw away esp
	cpu.esp += 32 / 8;
	//pop ebx
	opr.addr = cpu.esp;
	operand_read(&opr);
	cpu.ebx = opr.val;
	cpu.esp += 32 / 8;
	//pop edx
	opr.addr = cpu.esp;
	operand_read(&opr);
	cpu.edx = opr.val;
	cpu.esp += 32 / 8;
	//pop ecx
	opr.addr = cpu.esp;
	operand_read(&opr);
	cpu.ecx = opr.val;
	cpu.esp += 32 / 8;
	//pop eax
	opr.addr = cpu.esp;
	operand_read(&opr);
	cpu.eax = opr.val;
	cpu.esp += 32 / 8;

	print_asm_0("popa", "", 1);
	return 1;

}
