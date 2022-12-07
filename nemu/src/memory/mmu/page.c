#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
/*	printf("\nPlease implement page_translate()\n");
	fflush(stdout);
	assert(0);*/
	
	uint32_t dir = laddr >> 22;
	uint32_t page = (laddr & 0x3fffff) >> 12;
	uint32_t offset = laddr & 0xfff;
	
	PDE *pageDir = (PDE*)(hw_mem + (cpu.cr3.pdbr << 12) + (dir << 2));
	
	PTE *pageTable = (PTE*)(hw_mem + (pageDir->page_frame << 12) + (page << 2));
	assert(pageDir->present == 1);
	assert(pageTable->present == 1);
	
	return (pageTable->page_frame << 12) + offset;
	
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
