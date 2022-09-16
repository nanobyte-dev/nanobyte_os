#include "Stage2Allocator.hpp"

Stage2Allocator::Stage2Allocator(void* base, uint32_t limit)
    : m_Base(reinterpret_cast<uint8_t*>(base)), m_Limit(limit)
{
}

void* Stage2Allocator::Allocate(size_t size)
{
    if (m_Allocated + size >= m_Limit)
        return nullptr;

    void* ret = m_Base + m_Allocated;
    m_Allocated += size;
    return ret;
}

void Stage2Allocator::Free(void* addr)
{
    // NO-OP
}