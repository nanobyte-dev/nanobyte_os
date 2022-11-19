#pragma once
#include <stddef.h>
#include <core/Defs.hpp>

EXPORT void ASMCALL memcpy(void* dest, const void* src, size_t count);
EXPORT void * memset(void * ptr, int value, size_t num);

namespace Memory
{
    constexpr auto Copy = memcpy;
    constexpr auto Set = memset;

} // namespace Memory
