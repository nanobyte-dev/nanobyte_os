#pragma once
#include <core/mem/Allocator.hpp>
#include <stdint.h>

class Stage2Allocator : public Allocator
{
public:
    Stage2Allocator(void* base, uint32_t limit);
    virtual void* Allocate(size_t size) override;
    virtual void Free(void* addr) override;

private:
    uint8_t* m_Base;
    uint32_t m_Allocated;
    uint32_t m_Limit;
};