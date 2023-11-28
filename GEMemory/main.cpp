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

    {
        std::cout << "Testing threadsafe stack allocator with " << TEST_NUM_THREAD_REGIONS << " allocators and " << TEST_NUM_THREADS << " threads\n";
        auto f = [](uint32_t id, ThreadsafeAllocator* allocator)
        {
            MemRegion mem = allocator->Alloc(4, id);
            if (!mem.IsValid())
            {
                std::cerr << "ERROR: threaded memory region recieved faulty!\n";
            }
            else
            {
                for (uint32_t i = 0; i < 100; i++)
                {
                    mem.Write(&i, 4);
                    if (*((uint32_t*)mem.Read()) != i)
                    {
                        std::cerr << "ERROR: threaded memory write read error!\n";
                    }
                }
            }
            allocator->Free(&mem, id);
        };

        std::thread testThreads[TEST_NUM_THREADS];

        ThreadsafePoolAlloc threadsafeallocator(TEST_NUM_THREAD_REGIONS, DEFAULT_MEM_SIZE, 4);

        for (uint32_t i = 0; i < TEST_NUM_THREAD_REGIONS; i++)
        {
            std::cout << "Allocator " << i << " starting used memory: " << threadsafeallocator.CurrentStored(i) << std::endl;
        }

        for (uint32_t i = 0; i < TEST_NUM_THREADS; i++)
        {
            testThreads[i] = std::thread(f, i % TEST_NUM_THREAD_REGIONS, &threadsafeallocator);
        }
        for (uint32_t i = 0; i < TEST_NUM_THREADS; i++)
        {
            testThreads[i].join();
        }
        for (uint32_t i = 0; i < TEST_NUM_THREAD_REGIONS; i++)
        {
            std::cout << "Allocator " << i << " used memory: " << threadsafeallocator.CurrentStored(i) << std::endl;
        }
    }


    return 0;
}
