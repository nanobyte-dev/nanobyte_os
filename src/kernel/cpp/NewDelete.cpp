#include <stddef.h>
#include <core/mem/Allocator.hpp>

Allocator* g_CppAllocator;

void SetCppAllocator(Allocator* cppAllocator)
{
    g_CppAllocator = cppAllocator;
}

void *operator new(size_t size) throw()
{
    if (!g_CppAllocator)
        return nullptr;

    return g_CppAllocator->Allocate(size);
}
 
void *operator new[](size_t size) throw()
{
    if (!g_CppAllocator)
        return nullptr;

    return g_CppAllocator->Allocate(size);
}
 
void operator delete(void *p)
{
    if (g_CppAllocator)
        g_CppAllocator->Free(p);
}

void operator delete[](void *p)
{
    if (g_CppAllocator)
        g_CppAllocator->Free(p);
}