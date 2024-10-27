#pragma once

#include <stdint.h>
#include <stddef.h>
#include <libc_defs.h>

int ASMCALL memcmp_str(const void* s1, const void* s2, size_t count);
int ASMCALL memcmp_mmx(const void* s1, const void* s2, size_t count);

inline int memcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp_str(s1, s2, n);
}