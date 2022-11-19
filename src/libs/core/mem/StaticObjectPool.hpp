#pragma once
#include <stddef.h>

template <typename T, size_t PoolSize>
class StaticObjectPool
{
public:
    StaticObjectPool();
    T* Allocate();
    void Free(T* object);

private:
    T m_Objects[PoolSize] {};
    bool m_ObjectInUse[PoolSize] {};
    size_t m_Size;
};

template <typename T, size_t PoolSize>
StaticObjectPool<T, PoolSize>::StaticObjectPool()
    : m_Size(0)
{
    for (size_t i = 0; i < PoolSize; i++)
        m_ObjectInUse[i] = false;
}

template <typename T, size_t PoolSize>
T* StaticObjectPool<T, PoolSize>::Allocate()
{
    if (m_Size >= PoolSize)
        return nullptr;

    for (size_t i = 0; i < PoolSize; i++)
    {
        size_t idx = (i + m_Size) % PoolSize;
        if (!m_ObjectInUse[idx])
        {
            m_ObjectInUse[idx] = true;
            m_Size++;
            return &m_Objects[idx];
        }
    }

    // should never happen
    return nullptr;
}

template <typename T, size_t PoolSize>
void StaticObjectPool<T, PoolSize>::Free(T* object)
{
    if (object < m_Objects || object >= m_Objects + PoolSize)
        return;

    size_t idx = object - m_Objects;
    m_ObjectInUse[idx] = false;
    m_Size--;
}
