#include "MemoryAlloc.h"
#include "AllocTester.h"

static void AllocTest()
{
    AllocTester tester;
    StackAlloc  stack(DEFAULT_MEM_SIZE);
    PoolAlloc   pool(DEFAULT_NODE_SIZE, DEFAULT_MEM_SIZE);
    StompAlloc stomp(DEFAULT_MEM_SIZE);
    BuddyAlloc  buddy(DEFAULT_BUDDY_MEM_SIZE);

    tester.Validate(stack, DEFAULT_NODE_SIZE, "Default Stack Test");
    tester.Validate(pool, DEFAULT_NODE_SIZE, "Default Pool Test", sizeof(uint8_t*));
    tester.TestStomp(stomp);
    tester.BuddyTest(buddy, 128, "Default Buddy Test");

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
    return 0;
}
