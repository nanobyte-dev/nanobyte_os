#pragma once

#include <stdint.h>
#include <stddef.h>
#include <libc_defs.h>

void* ASMCALL memchr_str(const void* ptr, int value, size_t num);
void* ASMCALL memchr_mmx(const void* ptr, int value, size_t num);

inline void* memchr(const void *ptr, int value, size_t num)
{
    return memchr_str(ptr, value, num);
}