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
    m_last  = m_start;
    m_freeNodes
        = (uint32_t) (m_memory->GetEnd() - m_memory->GetStart()) / (m_nodesize + m_headerSize);
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
    uint32_t iters
        = (uint32_t) (m_memory->GetEnd() - m_memory->GetStart()) / (m_nodesize + m_headerSize);
    for (uint32_t i = 0; i < iters; ++i)
    {
        memcpy_s(&validator, m_headerSize, node, m_headerSize);
        // validator = static_cast<uint8_t*>(header);
        if (validator == nullptr)
            break;

        node += (m_nodesize + m_headerSize);
        if (node > m_memory->GetEnd())
            node = m_memory->GetStart();
    }


    // Write header data for m_last
    memcpy_s(m_last, m_headerSize, &node, m_headerSize);
    m_last = node;
    // Write last block as occupied
    int32_t data = POOL_OCCUPIED;
    memcpy_s(m_last, m_headerSize, &data, m_headerSize);


    // Give data block
    --m_freeNodes;
    return MemRegion(m_last + m_headerSize, m_nodesize);
}


void PoolAlloc::Free(MemRegion* memory)
{
    uint8_t* tofind   = (memory->GetAtFree() - m_headerSize);
    uint8_t* node     = m_start;
    uint8_t* lastNode = node;

    // Find where memory is located
    while (node != nullptr && node != tofind)
    {
        // Save where we are
        lastNode = node;

        // Get next
        memmove_s(&node, m_headerSize, node, m_headerSize);
    }
    assert(!(node == nullptr) && "Memory has been lost.");

    // Case where we only need to remove
    if (node == m_last)
    {
        // "Remove" last element
        memset(node, 0, m_headerSize);

        // Make previous the new last element
        int32_t occupied = POOL_OCCUPIED;
        memcpy_s(lastNode, m_headerSize, &occupied, m_headerSize);
        m_last = lastNode;
        ++m_freeNodes;
        return;
    }

    // Move start to first node in chain
    if (node == m_start)
    {
        uint8_t* next = nullptr;
        memmove_s(&next, m_headerSize, node, m_headerSize);
        memset(node, 0, m_headerSize);
        if (next != nullptr)
            m_start = next;
        ++m_freeNodes;
        return;
    }

    // "Glue" together two nodes
    memcpy_s(lastNode, m_headerSize, node, m_headerSize);
    memset(node, 0, m_headerSize);
    ++m_freeNodes;
}

size_t PoolAlloc::GetBlockSize() const { return m_nodesize; }
