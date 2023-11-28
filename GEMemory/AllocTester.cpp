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
    std::printf("[+] Starting validation test for \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";
    // Time
    auto tstart = high_resolution_clock::now();
    if (_Validate(subject, allocSize, testName, headerSize) != 0)
        return;
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
                             const char*          testName) const
{
    std::printf("[+] Starting thread test for \"%s\"...\n", testName);
    std::cout << "========================= Start =========================\n";
    // Time
    auto tstart = high_resolution_clock::now();
    if (_ThreadTest(subject, allocSize, numThreadRegions, testName) != 0)
        return;
    // Only print if function succeds
    std::cout << "========================= End =========================\n";
    std::printf("Thread test took %8.8lld micro seconds\n",
                duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
    std::printf("[+] Done with test \"%s\".\n\n\n", testName);
}
int AllocTester::_ThreadTest(ThreadsafeAllocator& subject,
                             const size_t         allocSize,
                             const uint32_t       numThreadRegions,
                             const char*          testName) const
{
    std::cout << "Testing threadsafe stack allocator with " << numThreadRegions
              << " allocators and " << TEST_NUM_THREADS << " threads\n";
    auto f = [&](uint32_t id, ThreadsafeAllocator* allocator)
    {
        MemRegion mem = allocator->Alloc(allocSize, id);
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
        }
        allocator->Free(&mem, id);
    };


    std::thread testThreads[TEST_NUM_THREADS];

    for (uint32_t i = 0; i < numThreadRegions; i++)
    {
        std::cout << "Allocator " << i << " starting used memory: " << subject.CurrentStored(i)
                  << std::endl;
    }

    for (uint32_t i = 0; i < TEST_NUM_THREADS; i++)
    {
        testThreads[i] = std::thread(f, i % numThreadRegions, &subject);
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


void AllocTester::Benchmark(Allocator& subject, size_t allocSize, const char* testName) const
{
    // Time
    auto tstart = high_resolution_clock::now();
    if (_Benchmark(subject, allocSize, testName) != 0)
        return;
    // Only print if function succeds
    std::printf("Benchmark test took %8.8lld micro seconds\n",
                duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
}
int AllocTester::_Benchmark(Allocator& subject, size_t allocSize, const char* testName) const
{
    return 0;
}
