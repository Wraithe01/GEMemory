#include "DefaultAllocator.h"

MallocAlloc::MallocAlloc()
:   Allocator(42),
    m_activeMemory(0)
{}

MallocAlloc::MallocAlloc(size_t size)
:   Allocator(42),
    m_activeMemory(0)
{}

MemRegion MallocAlloc::Alloc(size_t itemSize)
{
    m_activeMemory += itemSize;
    return MemRegion(malloc(itemSize), itemSize);
}

void MallocAlloc::Free(MemRegion* memory)
{
    m_activeMemory -= memory->GetSize();
    free(memory->GetAtFree());
}

size_t MallocAlloc::CurrentStored()
{
    return m_activeMemory;
}
