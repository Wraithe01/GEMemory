#pragma once
#include "Allocator.h"
#include "Includes.h"

// [|next||Data|][|next||Data|]...
// ^ Visual representation of the data layout

// This is the pool allocator
class PoolAlloc : public Allocator
{
public:  // Methods
    PoolAlloc();
    PoolAlloc(uint32_t nodeSize);
    PoolAlloc(uint32_t nodeSize, size_t memSize);
    ~PoolAlloc();

    size_t GetBlockSize() const;

    // Returns a memory region for the user to read and write to,
    // this region is always zero-initialized. Only one block is returned
    // regardless of itemSize.
    [[nodiscard("Dropping MemRegion might result in memory leak.")]]
    virtual MemRegion Alloc(size_t itemSize) override;
    // Returns a memory region for the user to read and write to,
    // this region is always zero-initialized. Gives 1 block of given size.
    [[nodiscard("Dropping MemRegion might result in memory leak.")]]
    MemRegion Alloc();

    virtual void Free(MemRegion* memory) override;

    // Total size of used memory
    virtual size_t CurrentStored();

private:
    uint32_t IndexFromAddr(uint8_t* addr) const;
    uint8_t* AddrFromIndex(uint32_t index) const;

public: // variables
    static const uint32_t s_headerSize = sizeof(uint32_t);

private: 

    uint16_t m_nodeCount;
    uint32_t m_freeNodes;
    uint32_t m_nodeSize;
    uint8_t* m_next;
};
