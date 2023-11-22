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
    PoolAlloc(size_t nodeSize);
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

private:
public:  // variables
    uint8_t* m_start;
    uint8_t* m_last;

    uint32_t m_freeNodes;
    size_t   m_nodesize;

    static const uint32_t m_headerSize = sizeof(uint8_t*);
};
