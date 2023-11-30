#pragma once
#include "Allocator.h"
#include "ThreadSafeAllocators.h"
#include "BuddyAllocator.h"

class AllocTester
{
public:
    AllocTester();
    ~AllocTester();

    void Validate(Allocator&  subject,
                  size_t      allocSize,
                  const char* testName,
                  uint32_t    headerSize = 0) const;
    // Validates allocator robustness with threads making allocation requests and ensures usability of recieved memregions
    void ThreadTest(ThreadsafeAllocator& subject,
                    const size_t         allocSize,
                    const uint32_t       numThreadRegions,
                    const bool           queuedRequests,
                    const char*          testName) const;
    // benchmark test. 10000 allocs and frees in a pattern to trigger fragmentation
    void BenchmarkRandomAccess(Allocator& subject,
        size_t allocSize,
        const char* testName) const;
    // benchmark test. will perform 1000 allocs and then 1000 frees ten times, simulating 10000 memory requests in a stack ordered fashion
    void BenchmarkStackAccess(Allocator& subject,
        size_t allocSize,
        const char* testName) const;
    // benchmark test. will perform 1000 allocs and then flush ten times, simulating 10000 memory requests in a stack ordered fashion
    void BenchmarkStackAccess(StackAlloc& subject,
        size_t allocSize,
        const char* testName) const;
    void BuddyTest(BuddyAlloc& subject,
        const size_t allocSize,
        const char* testName);

    void TestStomp(Allocator& stomp) const;

private:
    int _Validate(Allocator&  subject,
                  size_t      allocSize,
                  const char* testName,
                  uint32_t    headerSize) const;
    int _ThreadTest(ThreadsafeAllocator& subject,
                    const size_t         allocSize,
                    const uint32_t       numThreadRegions,
                    const bool           queuedRequests,
                    const char*          testName) const;
    int _BenchmarkRandomAccess(Allocator& subject,
        size_t allocSize,
        const char* testName) const;
    int _BenchmarkStackAccess(Allocator& subject,
        size_t allocSize,
        const char* testName) const;
    int _BenchmarkStackAccess(StackAlloc& subject,
        size_t allocSize,
        const char* testName) const;
    int _BuddyTest(BuddyAlloc& subject,
        size_t allocSize,
        const char* testName) const;
};
