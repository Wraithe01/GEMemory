#include "StackAlloc.h"

StackAlloc::StackAlloc() { m_stackTop = m_memory->GetStart(); }
StackAlloc::StackAlloc(uint32_t memSize): Allocator(memSize)
{
    m_stackTop = m_memory->GetStart();
}
StackAlloc::~StackAlloc() {}


MemRegion StackAlloc::Alloc(size_t itemSize)
{
    if ((m_stackTop + itemSize) > m_memory->GetEnd())
        return MemRegion(nullptr, 0);
    memset(m_stackTop, 0, itemSize);
    m_stackTop += itemSize;
    return MemRegion((m_stackTop - itemSize), itemSize);
}

void StackAlloc::Free(MemRegion* memory) { m_stackTop = memory->GetAtFree(); }
void StackAlloc::Flush() { m_stackTop = m_memory->GetStart(); }
