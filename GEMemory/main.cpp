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


auto main(void) -> int
{
    AllocTest();
    return 0;
}
