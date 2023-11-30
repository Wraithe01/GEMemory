#include "AllocTester.h"
#include <chrono>
using namespace std::chrono;

AllocTester::AllocTester() {}
AllocTester::~AllocTester() {}


void AllocTester::Validate(Allocator&  subject,
                           size_t      allocSize,
                           const char* testName,
                           uint32_t    headerSize) const
{
    uint32_t err = 0;
    std::printf("[+] Starting validation test for \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";

    // Time
    auto tstart = high_resolution_clock::now();
    if ((err = _Validate(subject, allocSize, testName, headerSize)) != 0)
    {
        std::cout << "[-] Validate test failed with error code " << err << std::endl;
        return;
    }

    // Only print if function succeds
    std::cout << "========================= End =========================\n";
    std::printf("[+] Validate test took %8.8lld micro seconds\n",
                duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
    std::printf("[+] Done with test \"%s\".\n\n\n", testName);
}
int AllocTester::_Validate(Allocator&  subject,
                           size_t      allocSize,
                           const char* testName,
                           uint32_t    headerSize) const
{
    uint16_t fails = 0;

    std::cout << "This should not give any error messages.\n";
    MemRegion mem = subject.Alloc(DEFAULT_MEM_SIZE);
    if (!mem.IsValid())
    {
        ++fails;
        std::cout << "[-] Invalid memory\n";
    }
    subject.Free(&mem);
    if (fails > 0)
    {
        std::printf("[-] Allocation test failed with %i error messages.\n", fails);
        return 1;
    }

    std::cout << "This should send 1 error.\n";
    fails = 0;
    for (uint32_t i = 0; i < DEFAULT_MEM_SIZE / (allocSize + headerSize) + 1; ++i)
    {
        if (!subject.Alloc(allocSize).IsValid())
        {
            std::cout << "[-] Unable to allocate memory.\n";
            ++fails;
        }
    }
    if (fails > 1)
    {
        std::printf("[-] Capacity test failed with %i errors.\n", fails);
        return 2;
    }
    return 0;
}


void AllocTester::ThreadTest(ThreadsafeAllocator& subject,
                             const size_t         allocSize,
                             const uint32_t       numThreadRegions,
                             const bool           queuedRequests,
                             const char*          testName) const
{
    uint32_t err = 0;
    std::printf("[+] Starting thread test for \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";

    // Time
    auto tstart = high_resolution_clock::now();
    if ((err = _ThreadTest(subject, allocSize, numThreadRegions, queuedRequests, testName)) != 0)
    {
        std::cout << "[-] Thread test failed with error code " << err << std::endl;
        return;
    }

    // Only print if function succeds
    std::cout << "========================= End =========================\n";
    std::printf("Thread test took %8.8lld micro seconds\n",
                duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
    std::printf("[+] Done with test \"%s\".\n\n\n", testName);
}
int AllocTester::_ThreadTest(ThreadsafeAllocator& subject,
                             const size_t         allocSize,
                             const uint32_t       numThreadRegions,
                             const bool           queuedRequests,
                             const char*          testName) const
{
    std::cout << "Testing threadsafe allocator with " << numThreadRegions
              << " allocators and " << TEST_NUM_THREADS << " threads\n";
    auto f = [&](uint32_t id, ThreadsafeAllocator* allocator, bool enqueue)
    {
        MemRegion mem = enqueue ? allocator->EnqueueAlloc(allocSize, id) : allocator->Alloc(allocSize, id);
        if (!mem.IsValid())
        {
            std::cerr << "ERROR: threaded memory region recieved faulty!\n";
        }
        else
        {
            for (uint32_t i = 0; i < 100; i++)
            {
                mem.Write(&i, allocSize);
                if (*((uint32_t*) mem.Read()) != i)
                {
                    std::cerr << "ERROR: threaded memory write read error!\n";
                }
            }
            if (enqueue)
            {
                allocator->EnqueueFree(&mem, id);
            }
            else
            {
                allocator->Free(&mem, id);
            }
        }
    };


    std::thread testThreads[TEST_NUM_THREADS];

    for (uint32_t i = 0; i < numThreadRegions; i++)
    {
        std::cout << "Allocator " << i << " starting used memory: " << subject.CurrentStored(i)
                  << std::endl;
    }

    for (uint32_t i = 0; i < TEST_NUM_THREADS; i++)
    {
        testThreads[i] = std::thread(f, i % numThreadRegions, &subject, queuedRequests);
    }
    for (uint32_t i = 0; i < TEST_NUM_THREADS; i++)
    {
        testThreads[i].join();
    }
    for (uint32_t i = 0; i < numThreadRegions; i++)
    {
        std::cout << "Allocator " << i << " used memory: " << subject.CurrentStored(i) << std::endl;
    }
    return 0;
}


void AllocTester::BenchmarkRandomAccess(Allocator& subject, size_t allocSize, const char* testName) const
{
    std::printf("[+] Starting random alloc pattern benchmark \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";
    // Time
    auto tstart = high_resolution_clock::now();
    if (_BenchmarkRandomAccess(subject, allocSize, testName) != 0)
        return;
    // Only print if function succeds
    std::printf("Benchmark test took %8.8lld micro seconds\n",
        duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
    std::printf("[+] Done with test \"%s\".\n\n\n", testName);
}
void AllocTester::BenchmarkStackAccess(Allocator& subject, size_t allocSize, const char* testName) const
{
    std::printf("[+] Starting stack alloc pattern benchmark \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";
    // Time
    auto tstart = high_resolution_clock::now();
    if (_BenchmarkStackAccess(subject, allocSize, testName) != 0)
        return;
    // Only print if function succeds
    std::printf("Benchmark test took %8.8lld micro seconds\n",
        duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
    std::printf("[+] Done with test \"%s\".\n\n\n", testName);
}
void AllocTester::BenchmarkStackAccess(StackAlloc& subject, size_t allocSize, const char* testName) const
{
    std::printf("[+] Starting stack benchmark \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";
    // Time
    auto tstart = high_resolution_clock::now();
    if (_BenchmarkStackAccess(subject, allocSize, testName) != 0)
        return;
    // Only print if function succeds
    std::printf("Benchmark test took %8.8lld micro seconds\n",
        duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
    std::printf("[+] Done with test \"%s\".\n\n\n", testName);
}

int AllocTester::_BenchmarkRandomAccess(Allocator& subject, size_t allocSize, const char* testName) const
{
    // Begins by allocating 1000 memRegions
    MemRegion* holder = (MemRegion*)malloc(sizeof(MemRegion) * 1000);
    for (int i = 0; i < 1000; i++)
    {
        holder[i] = subject.Alloc(allocSize);
    }
    //performs 9000 fragmented frees and allocs
    for (int i = 0; i < 18; i++)
    {
        for (int x = 0; x < 100; x += 2)
        {
            for (int y = 0; y < 1000; y += 100)
            {
                subject.Free(&holder[y + x]);
            }
        }
        for (int j = 0; j < 1000; j += 2)
        {
            holder[j] = subject.Alloc(allocSize);
        }
    }
    // frees the 1000 memRegions
    for (int i = 0; i < 1000; i++)
    {
        subject.Free(&holder[i]);
    }
    free(holder);
    return 0;
}
int AllocTester::_BenchmarkStackAccess(Allocator& subject, size_t allocSize, const char* testName) const
{
    MemRegion* holder = (MemRegion*)malloc(sizeof(MemRegion) * 1000);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
            holder[j] = subject.Alloc(allocSize);
        }
        for (int j = 1000; j > 0; j--)
        {
            subject.Free(&holder[j - 1]);
        }
    }
    free(holder);
    return 0;
}
int AllocTester::_BenchmarkStackAccess(StackAlloc& subject, size_t allocSize, const char* testName) const
{
    MemRegion* holder = (MemRegion*)malloc(sizeof(MemRegion) * 1000);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
            holder[j] = subject.Alloc(allocSize);
        }
        // One benefit of the stack allocator is being able to clear all the memory with flush
        // no memregion has to free itself
        subject.Flush();
    }
    free(holder);
    return 0;
}



void AllocTester::BuddyTest(BuddyAlloc& subject, const size_t allocSize, const char* testName)
{
    uint32_t err = 0;
    std::printf("[+] Starting validation test for \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";

    // Time
    auto tstart = high_resolution_clock::now();
    if ((err = _BuddyTest(subject, allocSize, testName)) != 0)
    {
        std::cout << "[-] Validate test failed with error code " << err << std::endl;
        return;
    }

    // Only print if function succeds
    std::cout << "========================= End =========================\n";
    std::printf("[+] Validate test took %8.8lld micro seconds\n",
        duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
    std::printf("[+] Done with test \"%s\".\n\n\n", testName);
}

int AllocTester::_BuddyTest(BuddyAlloc& subject, size_t allocSize, const char* testName) const
{
    uint16_t fails = 0;

    std::cout << "This should not give any error messages.\n";
    MemRegion mem = subject.Alloc(DEFAULT_BUDDY_MEM_SIZE);
    if (!mem.IsValid())
    {
        ++fails;
        std::cout << "[-] Invalid memory\n";
    }
    subject.Free(&mem);
    if (fails > 0)
    {
        std::printf("[-] Allocation test failed with %i error messages.\n", fails);
        return 1;
    }

    std::cout << "This should send 1 error.\n";
    fails = 0;
    for (size_t i = 0; i < 2; i++)
    {
        if (!subject.Alloc(DEFAULT_BUDDY_MEM_SIZE).IsValid())
        {
            fails++;
            std::cout << "[-] Unable to allocate memory.\n";
        }
    }

    if (fails > 1)
    {
        std::printf("[-] Capacity test failed with %i errors.\n", fails);
        return 2;
    }
    return 0;
}

int AllocTester::_BenchmarkBuddy(BuddyAlloc& subject, size_t allocSize, const char* testName) const
{
    return 0;
}

void AllocTester::TestStomp(Allocator& stomp) const
{
    std::cout << "[+] Starting validation test for Default Stomp Test...\n";
    std::cout << "========================= Start =========================\n";
    MemRegion memory = stomp.Alloc(DEFAULT_NODE_SIZE);
    int data = 1337;
    memory.Write(&data, sizeof(int));

    std::cout << "This should not give any error messages.\n";
    stomp.Free(&memory);

    memory = stomp.Alloc(DEFAULT_NODE_SIZE);
    // Intentional underrun on sentinel
    MemRegion underAllocate = MemRegion(memory.GetAt() - 1, sizeof(int));
    underAllocate.Write(&data, sizeof(int));

    std::cout << "This should send 1 error.\n";
    stomp.Free(&memory);
    std::cout << "========================= End =========================\n";
    std::cout << "[+] Done with test STOMP SENTINEL \n\n\n";
}