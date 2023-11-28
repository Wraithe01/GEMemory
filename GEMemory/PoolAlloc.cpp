#include "PoolAlloc.h"

PoolAlloc::PoolAlloc()
: m_nodesize(DEFAULT_NODE_SIZE)
{
    m_start = m_memory->GetStart();
    m_last  = m_start;
    m_freeNodes
        = (uint32_t) (m_memory->GetEnd() - m_memory->GetStart()) / (m_nodesize + m_headerSize);
}
PoolAlloc::PoolAlloc(uint32_t nodeSize)
: m_nodesize(nodeSize)
{
    m_start = m_memory->GetStart();
    m_last  = m_start;
    m_freeNodes
        = (uint32_t) (m_memory->GetEnd() - m_memory->GetStart()) / (m_nodesize + m_headerSize);
}
PoolAlloc::PoolAlloc(uint32_t nodeSize, size_t memSize)
: Allocator(memSize)
, m_nodesize(nodeSize)
{
    m_start = m_memory->GetStart();
    m_last = m_start;
    m_freeNodes
        = (uint32_t)(m_memory->GetEnd() - m_memory->GetStart()) / (m_nodesize + m_headerSize);
}
PoolAlloc::~PoolAlloc() {}


MemRegion PoolAlloc::Alloc(size_t itemSize) { return Alloc(); }
MemRegion PoolAlloc::Alloc()
{
    if (m_freeNodes <= 0)
        return MemRegion(nullptr, 0);

    // Find free space to give
    uint8_t* node      = m_start;
    uint8_t* validator = nullptr;
    for (; node < m_memory->GetEnd(); node += (m_nodesize + m_headerSize))
    {
        memcpy_s(&validator, m_headerSize, node, m_headerSize);
        // validator = static_cast<uint8_t*>(header);
        if (validator == nullptr)
            break;
    }


    // Write header data for m_last
    memcpy_s(m_last, m_headerSize, &node, m_headerSize);
    m_last = node;

    // Give data block
    --m_freeNodes;
    return MemRegion(m_last + m_headerSize, m_nodesize);
}


void PoolAlloc::Free(MemRegion* memory)
{
    uint8_t* tofind   = (memory->GetAtFree() - m_headerSize);
    uint8_t* node     = m_start;
    uint8_t* lastNode = nullptr;

    // Find where memory is located
    do
    {
        // Save where we are
        lastNode = node;

        // Get next
        memmove_s(&node, m_headerSize, node, m_headerSize);
    } while (node != nullptr && node != tofind);
    assert(!(node == nullptr) && "Memory has been lost.");

    // Case where we only need to remove
    if (node == m_last)
    {
        memset(lastNode, 0, m_headerSize);
        m_last = lastNode;
        ++m_freeNodes;
        return;
    }

    // "Glue" together two nodes
    memcpy_s(lastNode, m_headerSize, node, m_headerSize);
    memset(node, 0, m_headerSize);
    ++m_freeNodes;
}

size_t PoolAlloc::CurrentStored()
{
    return ((uint32_t)(m_memory->GetEnd() - m_memory->GetStart()) / (m_nodesize + m_headerSize) - m_freeNodes) * m_nodesize;
}

size_t PoolAlloc::GetBlockSize() const { return m_nodesize; }
