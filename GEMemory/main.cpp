#include "MemoryAlloc.h"

// Probably best used for testing for now...
auto main(void) -> int
{
    {
        StackAlloc salloc;
        MemRegion  mem = salloc.Alloc(DEFAULT_MEM_SIZE);

        // Should not give any output
        if (!mem.IsValid())
            std::cout << "Invalid memory\n";
        salloc.Free(&mem);


        // Should print once
        size_t size = 64;
        for (uint32_t i = 0; i < DEFAULT_MEM_SIZE / size + 1; ++i)
        {
            if (!salloc.Alloc(size).IsValid())
                std::cout << "Unable to allocate memory.\n";
        }

        std::cout << "Done with Salloc.\n\n";
    }

    {
        PoolAlloc palloc;
        MemRegion mem = palloc.Alloc();

        // Should not give any output
        if (!mem.IsValid())
            std::cout << "Invalid memory\n";
        palloc.Free(&mem);


        // Should print once
        size_t size = DEFAULT_NODE_SIZE + sizeof(uint8_t*);
        for (uint32_t i = 0; i < DEFAULT_MEM_SIZE / size + 1; ++i)
        {
            if (!palloc.Alloc(size).IsValid())
                std::cout << "Unable to allocate memory.\n";
        }

        std::cout << "Done with Palloc.\n\n";
    }


    return 0;
}
