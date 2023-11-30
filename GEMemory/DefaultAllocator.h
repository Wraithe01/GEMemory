#pragma once
#include "Includes.h"
#include "Allocator.h"

// This is the baseline malloc allocator to compare performance to
class MallocAlloc : public Allocator
{
public:  // Methods
    MallocAlloc();
    MallocAlloc(size_t size);

    // Returns a memory region for the user to read and write to,
    // this region is always zero-initialized.
    [[nodiscard("Dropping MemRegion might result in memory leak.")]]
    virtual MemRegion Alloc(size_t itemSize) override;

    // Frees to memory
    virtual void Free(MemRegion* memory) override;

    // Total size of used memory
    virtual size_t CurrentStored();
private:
public:  // variables
    size_t m_activeMemory;
};
