#include "MemoryAlloc.h"
#include "AllocTester.h"

static void AllocTest()
{
    AllocTester tester;
    StackAlloc  stack(DEFAULT_MEM_SIZE);
    PoolAlloc   pool(DEFAULT_NODE_SIZE, DEFAULT_MEM_SIZE);

    tester.Validate(stack, DEFAULT_NODE_SIZE, "Default Stack Test");
    tester.Validate(pool, DEFAULT_NODE_SIZE, "Default Pool Test", sizeof(uint8_t*));


    ThreadsafePoolAlloc  tsPool(TEST_NUM_THREAD_REGIONS, DEFAULT_MEM_SIZE, DEFAULT_NODE_SIZE);
    ThreadsafeStackAlloc tsStack(TEST_NUM_THREAD_REGIONS, DEFAULT_MEM_SIZE);

    tester.ThreadTest(tsStack, 64, TEST_NUM_THREAD_REGIONS, "Thread Stack Test");
    tester.ThreadTest(tsPool, 64, TEST_NUM_THREAD_REGIONS, "Thread Pool Test");
}


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
        printf("Testing Buddy!\n");
      
        //MemRegion mem = balloc.Alloc(16);

        //if (!mem.IsValid())
        //{
          //  std::cout << "Invalid memory\n";
        //}
        //std::string wow = std::string("hello!");
        //mem.Write((void*)wow.c_str(), wow.size());
        //printf("Writing: %s\n", wow.c_str());
        //const uint8_t* read = mem.Read();
        //const char* content = (const char*) *read;
        //printf("Data read: %s\n", read);

        //balloc.Free(&mem);


        BuddyAlloc balloc(512);

        MemRegion mem1 = balloc.Alloc(64);

        if (!mem1.IsValid())
        {
            std::cout << "Unable to allocate memory 1.\n";
        }

        MemRegion mem2 = balloc.Alloc(8);
        if (!mem2.IsValid())
        {
            std::cout << "Unable to allocate memory 2.\n";
        }
        MemRegion mem3 = balloc.Alloc(128);

        if (!mem3.IsValid())
        {
            std::cout << "Unable to allocate memory 3.\n";
        }
        balloc.PrintBlocks();
        balloc.Free(&mem1);
        balloc.PrintBlocks();
        balloc.Free(&mem2);
        balloc.PrintBlocks();
        balloc.Free(&mem3);
        balloc.PrintBlocks();

        std::cout << "Done with Balloc.\n\n";
    }

    return 0;
}
