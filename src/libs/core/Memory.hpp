#pragma once
#include <stddef.h>
#include <core/Defs.hpp>
#include <string.h>

namespace Memory
{
    inline void* Copy(void* dest, const void* src, size_t count)
    {
        return ::memcpy(dest, src, count);
    }

    inline void* Set(void* ptr, int value, size_t num)
    {
        return ::memset(ptr, value, num);
    }

} // namespace Memory
