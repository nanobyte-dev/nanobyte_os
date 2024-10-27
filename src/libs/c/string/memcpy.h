#pragma once

#include <stddef.h>
#include <libc_defs.h>

void* ASMCALL memcpy_str(void* dest, const void* src, size_t count);
void* ASMCALL memcpy_mmx(void* dest, const void* src, size_t count);

inline void* memcpy(void* dest, const void* src, size_t count)
{
    return memcpy_str(dest, src, count);
}