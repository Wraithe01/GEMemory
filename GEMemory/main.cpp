#include "MemoryAlloc.h"

// Probably best used for testing for now...
auto main(void) -> int
{
    StackAlloc salloc;
    MemRegion  mem = salloc.Alloc(DEFAULT_MEM_SIZE);
    if (!mem.IsValid())
        std::cout << "Invalid memory\n";
    return 0;
}
