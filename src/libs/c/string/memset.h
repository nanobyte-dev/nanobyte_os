#pragma once

#include <stddef.h>
#include <libc_defs.h>

void* ASMCALL memset_str(void * ptr, int value, size_t num);
void* ASMCALL memset_mmx(void * ptr, int value, size_t num);

inline void* memset(void * ptr, int value, size_t num)
{
    return memset_str(ptr, value, num);
}