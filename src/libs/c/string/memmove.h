#pragma once

#include <stddef.h>
#include <libc_defs.h>

void* ASMCALL memmove_str(void* dest, const void* src, size_t count);
void* ASMCALL memmove_mmx(void* dest, const void* src, size_t count);

inline void* memmove(void* dest, const void* src, size_t count)
{
    return memmove_str(dest, src, count);
}