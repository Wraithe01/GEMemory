#include "PoolAlloc.h"

PoolAlloc::PoolAlloc()
: m_nodeSize(DEFAULT_NODE_SIZE + s_headerSize)
{
    // Calculate amount of valid blocks
    m_nodeCount = (uint16_t) (m_memory->GetSize() / m_nodeSize);
    m_freeNodes = m_nodeCount;

    // Initalise memory regions
    for (uint16_t i = 0; i < m_nodeCount; ++i)
    {
        uint32_t* data = (uint32_t*) AddrFromIndex(i);
        *data          = i + 1;
    }
    m_next = m_memory->GetStart();
}
PoolAlloc::PoolAlloc(uint32_t nodeSize)
: m_nodeSize(nodeSize + s_headerSize)
{
    // Calculate amount of valid blocks
    m_nodeCount = (uint16_t) (m_memory->GetSize() / m_nodeSize);
    m_freeNodes = m_nodeCount;

    // Initalise memory regions
    for (uint16_t i = 0; i < m_nodeCount; ++i)
    {
        uint32_t* data = (uint32_t*) AddrFromIndex(i);
        *data          = i + 1;
    }
    m_next = m_memory->GetStart();
}
PoolAlloc::PoolAlloc(uint32_t nodeSize, size_t memSize)
: Allocator(memSize)
, m_nodeSize(nodeSize + s_headerSize)
{
    // Calculate amount of valid blocks
    m_nodeCount = (uint16_t) (m_memory->GetSize() / m_nodeSize);
    m_freeNodes = m_nodeCount;

    // Initalise memory regions
    for (uint16_t i = 0; i < m_nodeCount; ++i)
    {
        uint32_t* data = (uint32_t*) AddrFromIndex(i);
        *data          = i + 1;
    }
    m_next = m_memory->GetStart();
}
PoolAlloc::~PoolAlloc() {}


MemRegion PoolAlloc::Alloc(size_t itemSize) { return Alloc(); }
MemRegion PoolAlloc::Alloc()
{
    MemRegion ret(nullptr, 0);
    if (m_freeNodes > 0)
    {
        ret = MemRegion((m_next + s_headerSize), (m_nodeSize - s_headerSize));
        --m_freeNodes;
        if (m_freeNodes != 0)
            m_next = AddrFromIndex(*((uint32_t*) m_next));
        else
            m_next = nullptr;
    }
    return ret;
}


void PoolAlloc::Free(MemRegion* memory)
{
    // Validate if memory in range

    ++m_freeNodes;
    uint8_t* data = memory->GetAtFree();
    if (m_next != nullptr)
    {
        (*(uint32_t*) data) = IndexFromAddr(m_next);
        m_next              = data;
        return;
    }
    // In case every block was allocated previously
    (*(uint32_t*) data) = m_nodeCount;
    m_next              = data;
}

size_t PoolAlloc::CurrentStored()
{
    return ((uint32_t) (m_memory->GetEnd() - m_memory->GetStart()) / (m_nodeSize) -m_freeNodes)
           * (m_nodeSize - s_headerSize);
}

const char* PoolAlloc::GetAllocName() const { return "Pool"; }

size_t PoolAlloc::GetBlockSize() const { return (m_nodeSize); }


uint32_t PoolAlloc::IndexFromAddr(uint8_t* addr) const
{
    return (uint32_t) ((addr - m_memory->GetStart()) / (m_nodeSize));
}
uint8_t* PoolAlloc::AddrFromIndex(uint32_t index) const
{
    return m_memory->GetStart() + (index * m_nodeSize);
}
