#pragma once
#include <stdint.h>

template <typename T>
uint32_t ToSegOffset(T addr)
{
    uint32_t addrInt = reinterpret_cast<uint32_t>(addr);
    uint32_t seg = (addrInt >> 4) & 0xFFFF;
    uint32_t off = addrInt & 0xF;
    return (seg << 16) | off;
}

template <typename T>
T ToLinear(uint32_t addr)
{
    uint32_t offset = (uint32_t)(addr) & 0xFFFF;
    uint32_t segment = (uint32_t)(addr) >> 16;
    return T(segment * 16 + offset);
}