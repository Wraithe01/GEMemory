#pragma once

#include "Allocator.h"
#include "Includes.h"

struct MemoryAllocationInfo {
    size_t size;
    uint64_t sentinel;
};

class StompAlloc : public Allocator {
public:
    StompAlloc();
    StompAlloc(uint32_t memSize);
    ~StompAlloc();

    [[nodiscard("Dropping MemRegion might result in memory leak.")]]
    virtual MemRegion Alloc(size_t itemSize) override;

    virtual void Free(MemRegion* memory) override;

private:
    size_t PAGE_SIZE = 4096;
    uint64_t SENTINEL_VALUE = 0xdeadbeefdeadbeef;
};