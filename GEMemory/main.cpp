#include "MemoryAlloc.h"
#include "AllocTester.h"

static void AllocTest()
{
    AllocTester tester;
    StackAlloc  stack(DEFAULT_MEM_SIZE);
    PoolAlloc   pool(DEFAULT_NODE_SIZE, DEFAULT_MEM_SIZE);
    BuddyAlloc  buddy(512);

    tester.Validate(stack, DEFAULT_NODE_SIZE, "Default Stack Test");
    tester.Validate(pool, DEFAULT_NODE_SIZE, "Default Pool Test", sizeof(uint8_t*));
    tester.Validate(buddy, 128, "Default Buddy Test");

    // for threaded stack tests, expect the chance of reading and writing failing as freeing is not coordinated with usage of memory
    ThreadsafePoolAlloc  tsPool(TEST_NUM_THREAD_REGIONS, DEFAULT_MEM_SIZE, DEFAULT_NODE_SIZE);
    ThreadsafeStackAlloc tsStack(TEST_NUM_THREAD_REGIONS, DEFAULT_MEM_SIZE);

    tester.ThreadTest(tsStack, 4, TEST_NUM_THREAD_REGIONS, false, "Thread Stack Direct Request Test");
    tester.ThreadTest(tsPool, 4, TEST_NUM_THREAD_REGIONS, false, "Thread Pool Direct Request Test");
    
    std::thread tsStackAgent1(&ThreadsafeAllocator::QueueAgent, &tsStack);
    std::thread tsPoolAgent1(&ThreadsafeAllocator::QueueAgent, &tsPool);
    tester.ThreadTest(tsStack, 4, TEST_NUM_THREAD_REGIONS, true, "Thread Stack Queued Request Test, Single Agent");
    tester.ThreadTest(tsPool, 4, TEST_NUM_THREAD_REGIONS, true, "Thread Pool Queued Request Test, Single Agent");
    
    std::thread tsStackAgent2(&ThreadsafeAllocator::QueueAgent, &tsStack);
    std::thread tsPoolAgent2(&ThreadsafeAllocator::QueueAgent, &tsPool);
    tester.ThreadTest(tsStack, 4, TEST_NUM_THREAD_REGIONS, true, "Thread Stack Queued Request Test, Two Agents");
    tester.ThreadTest(tsPool, 4, TEST_NUM_THREAD_REGIONS, true, "Thread Pool Queued Request Test, Two Agents");

    // Release and join the QueueAgent threads
    tsStack.TerminateAgent();
    tsStack.TerminateAgent();
    tsStackAgent1.join();
    tsStackAgent2.join();
    tsPool.TerminateAgent();
    tsPool.TerminateAgent();
    tsPoolAgent1.join();
    tsPoolAgent2.join();
}

auto main(void) -> int
{
    AllocTest();
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

        //BuddyAlloc balloc(512);

        //MemRegion mem1 = balloc.Alloc(128);

        //if (!mem1.IsValid())
        //{
        //    std::cout << "Unable to allocate memory 1.\n";
        //}

        //MemRegion mem2 = balloc.Alloc(32);
        //if (!mem2.IsValid())
        //{
        //    std::cout << "Unable to allocate memory 2.\n";
        //}
        //MemRegion mem3 = balloc.Alloc(8);

        //if (!mem3.IsValid())
        //{
        //    std::cout << "Unable to allocate memory 3.\n";
        //}
        //balloc.PrintBlocks();
        //balloc.Free(&mem1);
        //balloc.PrintBlocks();
        //balloc.Free(&mem2);
        //balloc.PrintBlocks();
        //balloc.Free(&mem3);
        //balloc.PrintBlocks();

        std::cout << "Done with Balloc.\n\n";
    }
    return 0;
}
