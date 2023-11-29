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

    buddyBlocks.push_back(initialBlock);
}

MemRegion BuddyAlloc::Alloc(size_t blockSize)
{ 
    size_t index = FindFreeBlock(blockSize);

    // If no suitable block is found we return an invalid MemRegion
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

    if (BUDDY_DEBUG)
    {
        printf("Allocated block with size %zu\n", blockSize);
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
        [memory](const BuddyBlock& block) { return block.start == memory->GetAt(); });

    if (it == buddyBlocks.end())
    {
        printf("Could not find that memory region in any block!\n");
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
        if (buddyBlocks[i].isFree && buddyBlocks[i].size >= requestedSize)
        {
            return i;
        }
    }
    // Could not find of the requested size
    return -1;
}

void BuddyAlloc::SplitBlock(size_t index)
{
    // We ensure the block can be split
    if (buddyBlocks[index].size <= 1) {
        return;
    }
    // We then split the block into two buddies
    size_t newSize = buddyBlocks[index].size / 2;
    printf("New split block size: %zu\n", newSize);

    BuddyBlock newBuddy1{};
    newBuddy1.start = buddyBlocks[index].start;
    newBuddy1.size = newSize;
    newBuddy1.isFree = true;

    BuddyBlock newBuddy2{};
    newBuddy2.start = buddyBlocks[index].start + newSize;
    newBuddy2.size = newSize;
    newBuddy2.isFree = true;

    // Remove old block
    buddyBlocks.erase(buddyBlocks.begin() + index);

    // Insert the new buddies
    buddyBlocks.insert(buddyBlocks.begin() + index, newBuddy1);
    buddyBlocks.insert(buddyBlocks.begin() + index + 1, newBuddy2);

    //buddyBlocks.push_back(newBuddy1);
    //buddyBlocks.push_back(newBuddy2);
}

// TODO: FIX!
void BuddyAlloc::MergeBlocks(size_t index)
{
    printf("Begin MERGE! Index: %zu\n", index);

    // Continue merging until a non-free neighbor block is encountered
    if (index < buddyBlocks.size() - 1 && buddyBlocks[index].isFree &&
        buddyBlocks[index + 1].isFree && buddyBlocks[index].size == buddyBlocks[index + 1].size)
    {
        uint8_t* start = buddyBlocks[index].start;
        size_t size = buddyBlocks[index].size;

        // Erase both current and right neighbor blocks
        buddyBlocks.erase(buddyBlocks.begin() + index, buddyBlocks.begin() + index + 2);

        // We add back the block with doubled size
        BuddyBlock mergedBlock{};
        mergedBlock.start = start;
        mergedBlock.size = 2 * size;
        mergedBlock.isFree = true;

        //buddyBlocks.push_back(mergedBlock);
        buddyBlocks.insert(buddyBlocks.begin() + index, mergedBlock);

        size_t parentIndex = index - 1;
        MergeBlocks(parentIndex);
    }
    printf("END MERGE!\n");
}

void BuddyAlloc::PrintBlocks()
{
    for (size_t i = 0; i < buddyBlocks.size(); i++)
    {
        printf("Block %zu, Size: %zu, Free: %s\n",
            i, buddyBlocks[i].size, buddyBlocks[i].isFree ? "True" : "False");
    }
}