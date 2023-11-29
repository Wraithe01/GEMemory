#pragma once
#include "Allocator.h"
#include "Includes.h"
#include <vector>
#include <algorithm>

constexpr bool BUDDY_DEBUG = true;

class BuddyAlloc : public Allocator
{
public:
    BuddyAlloc(size_t totalMemorySize);

    MemRegion Alloc(size_t blockSize) override;
    void Free(MemRegion* memory) override;

    void PrintBlocks();

private:
    struct BuddyBlock
    {
        uint8_t* start;
        size_t size;
        bool isFree;
        bool isSplit;
        bool isLeft;
    };

    std::vector<BuddyBlock> buddyBlocks;

    size_t FindFreeBlock(size_t blockSize);
    void SplitBlock(size_t index);
    void MergeBlocks(size_t index);
};