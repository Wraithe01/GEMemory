#include "MemoryAlloc.h"

// Probably best used for testing for now...
auto main(void) -> int
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

    return 0;
}
