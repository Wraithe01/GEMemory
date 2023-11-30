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
    void ThreadTest(ThreadsafeAllocator& subject,
                    const size_t         allocSize,
                    const uint32_t       numThreadRegions,
                    const bool           queuedRequests,
                    const char*          testName) const;
    void Benchmark(Allocator& subject,
        size_t allocSize,
        const char* testName) const;
    void BuddyTest(BuddyAlloc& subject,
        const size_t allocSize,
        const char* testName);

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
    int _Benchmark(Allocator& subject,
        size_t allocSize,
        const char* testName) const;
    int _BuddyTest(BuddyAlloc& subject,
        size_t allocSize,
        const char* testName) const;
};
