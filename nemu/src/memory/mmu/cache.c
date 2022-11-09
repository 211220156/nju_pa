#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <stdlib.h>
#include <time.h>

CacheLine cache[1024];
// init the cache
void init_cache()
{
	// implement me in PA 3-1
	for (int i = 0; i < 1024; i++)
	    cache[i].valid_bit = 0;
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
	//因为是write through，需要向主存里面写入。
	memcpy(hw_mem + paddr, &data, len);
	uint32_t sign = paddr >> 13;
	uint32_t group_num = (paddr >> 6) & 0x7f; //取组号
	uint32_t offset = paddr & 0x3f;
	for (int i = 0; i < 8; i++){
	    if (cache[group_num * 8 + i].sign == sign && cache[group_num * 8 + i].valid_bit){
	        //开始写
	        if (offset + len <= 64)//没跨行
	            memcpy(cache[group_num * 8 + i].block, &data, len);
	        else {
	            cache_write(paddr, 64 - offset, data);
	            cache_write(paddr + 64 - offset, offset + len - 64, data >> (8 * (64 - offset)));
	        }    
	        return;
	    }
	}
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	uint32_t sign = paddr >> 13;
	uint32_t group_num = (paddr >> 6) & 0x7f; //取组号
	uint32_t offset = paddr & 0x3f;
	uint32_t ans = 0;
	for (int i = 0; i < 8; i++){
	    if (cache[group_num * 8 + i].sign == sign && cache[group_num * 8 + i].valid_bit){
	        //开始读
	        if (offset + len <= 64)
	            memcpy(&ans, cache[group_num * 8 + i].block + offset, len);
	        else {
	            uint32_t num1 = 0, num2 = 0;
	            memcpy(&num1, cache[group_num * 8 + i].block + offset, 64 - offset);//先读当前行的,注意len单位是字节
	            num2 = cache_read(paddr - offset + 64, len + offset - 64) << (8 * (64 - offset));//小端方式，num2是高位
	            ans = num2 | num1;
	        }
	        return ans;
	    }
	}
	//若没有找到，即缓存没有命中
	memcpy(&ans, hw_mem + paddr, len);
	for (int i = 0; i < 8; i++){
	    if (!cache[group_num * 8 + i].valid_bit){//随机选一个空缺的cache行
	        cache[group_num * 8 + i].valid_bit = 1;
	        cache[group_num * 8 + i].sign = sign;
	        memcpy(cache[group_num * 8 + i].block, hw_mem + paddr - offset, 64);
	        return ans;
	    }
	}
	//若没找到valid_bit为0的cache行，则在组内随机选一行替换掉。
	srand(time(0));
	int num = rand() % 8;
	cache[group_num * 8 + num].valid_bit = 1;
	cache[group_num * 8 + num].sign = sign;
	memcpy(cache[group_num * 8 + num].block, hw_mem + paddr - offset, 64);
	return ans;
}
