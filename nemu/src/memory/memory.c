#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

#include "memory/mmu/cache.h"

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len) 
{
	uint32_t ret = 0;
#ifdef CACHE_ENABLED
	ret = cache_read(paddr, len);     // 通过cache进行读
#else
	ret = hw_mem_read(paddr, len);
#endif
	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data) 
{
#ifdef CACHE_ENABLED
	cache_write(paddr, len, data);    // 通过cache进行写
#else
	hw_mem_write(paddr, len, data);
#endif
}


uint32_t laddr_read(laddr_t laddr, size_t len)
{
    assert(len == 1 || len == 2 || len == 4);
    uint32_t paddr = laddr;
    if (cpu.cr0.pg && cpu.cr0.pe){//开启分页机制
        if((laddr >> 12) != ((laddr + (len << 3)) >> 12)){
	        uint32_t len1 = (((laddr >> 12) + 1) << 12) - laddr;
	        uint32_t low = paddr_read(page_translate(laddr), len1);
	        uint32_t high = paddr_read(page_translate((((laddr >> 12) + 1) << 12)), len - len1);
	        return (high << (8 * len1)) + low;
	    } 
	    else
            paddr = page_translate(laddr);
    }
	return paddr_read(paddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
    uint32_t paddr = laddr;
	if(cpu.cr0.pe && cpu.cr0.pg) {
	    if((laddr >> 12) != ((laddr + (len << 3)) >> 12)){
	        uint32_t len1 = (((laddr >> 12) + 1) << 12) - laddr;
	        uint32_t low = data & (0xffffffff >> (32 - (len1 << 3)));
	        uint32_t high = data >> (32 - (len1 << 3));
	        paddr_write(page_translate(laddr), len1, low);
	        paddr_write(page_translate((((laddr >> 12) + 1) << 12)), len - len1, high);
	        return;
	    }
	    else
		    paddr = page_translate(laddr);
	}
	paddr_write(paddr, len, data);
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	return laddr_read(vaddr, len);
#else
	uint32_t laddr = vaddr;
	if(  cpu.cr0.pe == 1 ) {
		laddr = segment_translate(vaddr, sreg);
	}
	return laddr_read(laddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	laddr_write(vaddr, len, data);
#else
	uint32_t laddr = vaddr;
	if(  cpu.cr0.pe == 1 ) {
		laddr = segment_translate(vaddr, sreg);
	}
	laddr_write(laddr, len, data);
#endif

}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
#ifdef CACHE_ENABLED
	init_cache();                             // 初始化cache
#endif
#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
