#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	uint32_t base = cpu.segReg[sreg].base;
	
	return offset + base;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	uint32_t addr = (uint32_t)hw_mem + cpu.gdtr.base + cpu.segReg[sreg].index * sizeof(SegDesc);
	SegDesc* curr = (void*)addr;
	assert(curr->present == 1 && curr->granularity == 1);
	uint32_t base = (curr->base_31_24 << 24) + (curr->base_23_16 << 16) + curr->base_15_0;
	uint32_t limit = (curr->limit_19_16 << 16) + curr->limit_15_0;
	assert(limit == 0xfffff && base == 0);
	cpu.cache[sreg].base = base;
	cpu.cache[sreg].limit = limit;
	cpu.cache[sreg].privilege_level = curr->privilege_level;
}
