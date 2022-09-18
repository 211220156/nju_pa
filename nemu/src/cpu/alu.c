#include "cpu/cpu.h"
/**********************alu_add实现****************************************/
void set_CF_add(uint32_t res, uint32_t src, size_t data_size)
{
    res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
    
    cpu.eflags.CF = res < src;
}

void set_PF(uint32_t res)
{
    int ans = 0;
    for (int i = 0; i < 8; i++) {
        if (((res >> i) & 1) == 1)
            ans++;
    }
    if (ans % 2 == 0)
        cpu.eflags.PF = 1;
    else
        cpu.eflags.PF = 0;
}

void set_ZF(uint32_t res, size_t data_size)
{
    res = res & (0xFFFFFFFF >> (32 - data_size));
    cpu.eflags.ZF = res == 0;
}

void set_SF(uint32_t res, size_t data_size)
{
//    res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)) ,data_size);
//    cpu.eflags.SF = sign(res);
    res = res & (0xFFFFFFFF >> (32 - data_size));
    cpu.eflags.SF = (res >> (data_size - 1)) & 1;
}

void set_OF_add(uint32_t res, uint32_t src, uint32_t dest, size_t data_size)
{
    res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    if (sign(dest) == sign(src)) {
        if (sign(src) == sign(res)) {
            cpu.eflags.OF = 0;
        } else {
            cpu.eflags.OF = 1;
        }
    } else {
        cpu.eflags.OF = 0;
    }
}

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest + src;
	
	set_CF_add(res, src, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_add(res, src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));//高位清零
	
#endif
}


/************************alu_adc实现*********************************/
void set_CF_adc(uint32_t res, uint32_t src, size_t data_size)
{
    res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
    
    if ((cpu.eflags.CF == 1 && res <= src) || (cpu.eflags.CF == 0 && res < src)){
        cpu.eflags.CF = 1;
    } else {
        cpu.eflags.CF = 0;
    }
    
}

void set_OF_adc(uint32_t res, uint32_t src, uint32_t dest, size_t data_size)
{
    set_OF_add(res, src, dest, data_size);
}


uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src + dest + cpu.eflags.CF;
	
	set_CF_adc(res, src, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_adc(res, src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));//高位清零
	
	
#endif
}

/*************************alu_sub实现*************************/
void set_CF_sub(uint32_t res, uint32_t dest, size_t data_size)
{
    res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    
    cpu.eflags.CF = res > dest;
}

void set_OF_sub(uint32_t res, uint32_t src, uint32_t dest, size_t data_size)
{
    res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    
    if (sign(dest) != sign(src)) {
        if (sign(res) != sign(dest)) {
            cpu.eflags.OF = 1;
        } else {
            cpu.eflags.OF = 0;
        }
    } else {
        cpu.eflags.OF = 0;
    }
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	
	uint32_t res = 0;
	res = dest - src;//sign_ext(src & (src >> (32 - data_size)), data_size);
	
	set_CF_sub(res, dest, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_sub(res, src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));//高位清零
	
#endif
}
/*************************alu_sbb实现*************************/
void set_CF_sbb(uint32_t res, uint32_t dest, size_t data_size)
{
    if (cpu.eflags.CF == 0) {
        set_CF_sub(res, dest, data_size);
    } else {
        res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
        dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
        
        cpu.eflags.CF = res >= dest;
    }
}

void set_OF_sbb(uint32_t res, uint32_t src, uint32_t dest, size_t data_size)
{
    set_OF_sub(res, src, dest, data_size);
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	
	uint32_t res = 0;
	res = dest - (src + cpu.eflags.CF);
	
	set_CF_sbb(res, dest, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_sbb(res, src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));//高位清零
	
#endif
}
/*************************alu_mul实现*************************/
uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	
	src = src & (0xFFFFFFFF >> (32 - data_size));
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    uint64_t res = 0, dest_64 = dest, src_64 = src;
    res = (dest_64 * src_64) & (0xFFFFFFFFFFFFFFFF >> (64 - 2 * data_size));
    switch (data_size){
        case 8: {
            uint64_t AH = res >> 8;
            if (AH == 0) {
                cpu.eflags.CF = 0;
                cpu.eflags.OF = 0;
            } else {
                cpu.eflags.CF = 1;
                cpu.eflags.OF = 1;
            }
        }
        case 16: {
            uint64_t DX = res >> 16;
            if (DX == 0) {
                cpu.eflags.CF = 0;
                cpu.eflags.OF = 0;
            } else {
                cpu.eflags.CF = 1;
                cpu.eflags.OF = 1;
            }
        }
        default:{
            uint64_t EDX = res >> 32;
            if (EDX == 0) {
                cpu.eflags.CF = 0;
                cpu.eflags.OF = 0;
            } else {
                cpu.eflags.CF = 1;
                cpu.eflags.OF = 1;
            }
        }
    }
    return res;
    
	
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}
/*************************alu_and实现*************************/
uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else

	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	src = src & (0xFFFFFFFF >> (32 - data_size));
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    uint32_t res = 0;
    res = dest & src;
    set_PF(res);
    set_SF(res, data_size);
    set_ZF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}
/*************************alu_xor实现*************************/
uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else

	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	src = src & (0xFFFFFFFF >> (32 - data_size));
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    uint32_t res = 0;
    res = dest ^ src;
    set_PF(res);
    set_SF(res, data_size);
    set_ZF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}
/*************************alu_or实现*************************/
uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	src = src & (0xFFFFFFFF >> (32 - data_size));
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    uint32_t res = 0;
    res = dest | src;
    set_PF(res);
    set_SF(res, data_size);
    set_ZF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}
/*************************alu_shl实现 逻辑左移*************************/
uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	//先将高位清零
	src = src & (0xFFFFFFFF >> (32 - data_size));
	dest = dest & (0xFFFFFFFF >> (32 - data_size));
    cpu.eflags.CF = (dest >> (data_size - src)) % 2; //CF就是dest从右往左数第data_size+1-src位。
    uint32_t res = (dest << src) & (0xFFFFFFFF >> (32 - data_size));//左移后高位清零
    set_SF(res, data_size);
    set_PF(res);
    set_ZF(res, data_size);
    return res;
	
#endif
}
/*************************alu_shr实现 逻辑右移*************************/
uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	
	src = src & (0xFFFFFFFF >> (32 - data_size));
	dest = dest & (0xFFFFFFFF >> (32 - data_size));
	cpu.eflags.CF = (dest >> (src - 1)) % 2;//CF是第src位。
	uint32_t res = dest >> src;
	set_SF(res, data_size);
    set_PF(res);
    set_ZF(res, data_size);
    return res;
	
#endif
}
/*************************alu_sar实现 算术右移*************************/
uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	
	src = src & (0xFFFFFFFF >> (32 - data_size));
	dest = dest & (0xFFFFFFFF >> (32 - data_size));
	cpu.eflags.CF = (dest >> (src - 1)) % 2;//CF是第src位。
	int sign = (dest >> (data_size - 1)) % 2;
	uint32_t res = 0;
	uint32_t temp = (0xFFFFFFFF >> (data_size - src)) << (data_size - src);//高位置1
	if (sign == 1) {
	    res = (dest >> src) | temp;
	} else {
	    res = dest >> src;
	}
	res = res & (0xFFFFFFFF >> (32 - data_size));
	set_SF(res, data_size);
    set_PF(res);
    set_ZF(res, data_size);
    return res;
	
#endif
}
/*************************alu_sal实现 算术左移*************************/
uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	
	return alu_shl(src, dest, data_size);
	
#endif
}
