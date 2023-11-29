#include "BuddyAllocator.h"

BuddyAlloc::BuddyAlloc(size_t totalMemorySize) : Allocator(totalMemorySize)
{
    if (BUDDY_DEBUG)
    {
        printf("Allocated block with %zu\n", totalMemorySize);
    }
    BuddyBlock initialBlock{};
    initialBlock.start = m_memory->GetStart();
    initialBlock.size = m_memory->GetSize();
    initialBlock.isFree = true;
    initialBlock.isSplit = false;
    initialBlock.isLeft = true;

    buddyBlocks.push_back(initialBlock);
}

MemRegion BuddyAlloc::Alloc(size_t blockSize)
{ 
    // Make sure the block size is power of 2
    if ((blockSize & (blockSize - 1)) != 0) {
        printf("Block size must be a power of 2!\n");
        return MemRegion(nullptr, 0);
    }
    size_t index = FindFreeBlock(blockSize);

    if (index == -1)
    {
        printf("Could not find a free block for size %zu\n", blockSize);
        return MemRegion(nullptr, 0);
    }
    // We split the block if needed
    while (buddyBlocks[index].size >= blockSize * 2) {
        SplitBlock(index);
    }
    // Mark the block as allocated
    buddyBlocks[index].isFree = false;

    PrintBlocks();

    if (BUDDY_DEBUG)
    {
        printf("Allocated block with size %zu\n", buddyBlocks[index].size);
    }
    return MemRegion(buddyBlocks[index].start, buddyBlocks[index].size);
}

void BuddyAlloc::Free(MemRegion* memory)
{
    if (!memory || !memory->IsValid()) {
        printf("Could not free memory because the memory is either null or already freed!");
        return;
    }

    // We find the block corresponding to the memory region
    auto it = std::find_if(buddyBlocks.begin(), buddyBlocks.end(),
        [memory](const BuddyBlock& block) { return block.start == memory->GetAt() && 
        block.size == memory->GetSize() && !block.isSplit; });

    if (it == buddyBlocks.end())
    {
        printf("Could not find that memory region in any block! Has it been freed already?\n");
        return;
    }
    // We mark the block as free!
    it->isFree = true;

    // Merge adjacent free blocks
    MergeBlocks(std::distance(buddyBlocks.begin(), it));
}

size_t BuddyAlloc::FindFreeBlock(size_t requestedSize)
{
    // We try to find the smallest available block that we can fit the requested data size into
    for (size_t i = 0; i < buddyBlocks.size(); ++i)
    {
        if (buddyBlocks[i].isFree && buddyBlocks[i].size >= requestedSize && !buddyBlocks[i].isSplit)
        {
            return i;
        }
    }
    return -1;
}

void BuddyAlloc::SplitBlock(size_t index)
{
    // We ensure the block can be split
    if (buddyBlocks[index].size <= 1) {
        return;
    }
    buddyBlocks[index].isSplit = true;
    buddyBlocks[index].isFree = false;

    // We then split the block into two buddies
    uint8_t* start = buddyBlocks[index].start;
    size_t newSize = buddyBlocks[index].size / 2;
    printf("New split block size: %zu\n", newSize);

    BuddyBlock newBuddy1{};
    newBuddy1.start = start;
    newBuddy1.size = newSize;
    newBuddy1.isFree = true;
    newBuddy1.isLeft = true;

    BuddyBlock newBuddy2{};
    newBuddy2.start = start + newSize;
    newBuddy2.size = newSize;
    newBuddy2.isFree = true;
    newBuddy2.isLeft = false;

    // Insert the new buddies
    buddyBlocks.insert(buddyBlocks.begin() + index, newBuddy1);
    buddyBlocks.insert(buddyBlocks.begin() + index + 1, newBuddy2);
}

void BuddyAlloc::MergeBlocks(size_t index)
{
    printf("Begin MERGE! Index: %zu\n", index);

    while (buddyBlocks.size() > 1) {
        size_t buddy = index + (buddyBlocks[index].isLeft ? +1 : -1);

        // Continue merging until a non-free neighbor block is encountered
        if (index < buddyBlocks.size() - 1 && buddyBlocks[index].isFree
            && buddyBlocks[buddy].isFree
            && buddyBlocks[index].size == buddyBlocks[buddy].size
            && !buddyBlocks[buddy].isSplit)
        {
            size_t leftIndex = std::min(index, buddy);
            size_t rightIndex = std::max(index, buddy);
            size_t parentIndex = leftIndex + 2;

            buddyBlocks[parentIndex].isSplit = false;
            buddyBlocks[parentIndex].isFree = true;

            buddyBlocks.erase(buddyBlocks.begin() + leftIndex, buddyBlocks.begin() + rightIndex + 1);

            PrintBlocks();
            // Update index after merge
            index = parentIndex - 2;
        }
        else {
            // Break the loop if no merge is possible
            break;
        }
    }
    printf("END MERGE!\n");
}

void BuddyAlloc::PrintBlocks()
{
    for (size_t i = 0; i < buddyBlocks.size(); i++)
    {
        printf("Block %zu, Size: %zu, Free: %s, Split: %s, Left: %s\n",
            i, buddyBlocks[i].size, buddyBlocks[i].isFree ? "True" : "False", buddyBlocks[i].isSplit ? "True" : "False", buddyBlocks[i].isLeft ? "True" : "False");
    }
}