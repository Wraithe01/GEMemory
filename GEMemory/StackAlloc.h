#pragma once
#include "Allocator.h"
#include "Includes.h"

// This is the stack allocator
class StackAlloc : public Allocator
{
public:  // Methods
    StackAlloc();
    ~StackAlloc();

    virtual MemRegion& Alloc(void* item, size_t itemSize) override;
    virtual void       Free(void* start) override;

private:
public:  // variables
};
