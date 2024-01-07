#pragma once
#include "Allocator.h"
#include "Includes.h"

// This is the stack allocator
class StackAlloc : public Allocator
{
public:  // Methods
    StackAlloc();
    StackAlloc(uint32_t memSize);
    ~StackAlloc();

    // Returns a memory region for the user to read and write to,
    // this region is always zero-initialized.
    [[nodiscard("Dropping MemRegion might result in memory leak.")]]
    virtual MemRegion Alloc(size_t itemSize) override;

    // For the stack allocator, using Free on a item that is not on the top
    // of the stack, will result in every element above it in the stack to be
    // flushed as well.
    virtual void Free(MemRegion* memory) override;

    // Total size of used memory
    virtual size_t CurrentStored();
    virtual size_t GetCapacity();

    // Flush all elements in the stack.
    // Any existing MemRegions will be nulled at this point, UB with continued use.
    void Flush();

    virtual const char* GetAllocName() const override;

private:
public:  // variables
    uint8_t* m_stackTop;
};
