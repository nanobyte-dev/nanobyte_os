#pragma once
#include <stddef.h>
#include <core/Defs.hpp>

EXPORT void ASMCALL memcpy(void* dest, void* src, size_t count);

namespace Memory
{
    constexpr auto Copy = memcpy;

} // namespace Memory
