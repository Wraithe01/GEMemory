#pragma once

#include "Includes.h"
#include "Allocator.h"
#include "PoolAlloc.h"
#include "StackAlloc.h"
#include "DefaultAllocator.h"

// Struct used internally for requestQueue
struct AllocatorRequest
{
    // 0 for alloc, 1 for free and 2 for terminate QueueAgents
    int8_t type;
    // Returned memregion for alloc
    MemRegion* returnMemory;
    // Condition variable for individual threads to know they've recieved a memregion
    bool* processed;
    // InputParameters
    size_t itemSize;
    uint32_t regionID;
    MemRegion memory;
};

// This is the base class for threadsafe allocator encapsulaion
class ThreadsafeAllocator
{
public: // Methods
    ThreadsafeAllocator() = delete;
    // ThreadsafeAllocator base class may be instantiated by itself, allowing for more advanced usage
    // The regionAllocators may be any combination of allocators using any size of memory blocks
    // However the more concrete memory allocators are recommended for automatic configuration and freeing of the allocators 
    ThreadsafeAllocator(uint32_t regions, Allocator** regionAllocators);
    ~ThreadsafeAllocator();

    // Encapsulates the allocation of memory from the allocator belonging to a specific region ID
    // Region IDs may be specifically belonging to a certain thread or shared between multiple
    // Region ID is simply the index of the region
    [[nodiscard("Dropping MemRegion might result in memory leak.")]]
    MemRegion Alloc(size_t itemSize, uint32_t regionID);

    // Encapsulates the allocation of memory from the allocator belonging to a specific region ID
    // Region IDs may be specifically belonging to a certain thread or shared between multiple
    // Region ID is simply the index of the region
    void Free(MemRegion* memory, uint32_t regionID);

    uint32_t CurrentStored(uint32_t regionID);

    // Enqueues request for allocator, lowering chance for starvation
    // All memory regions share one queue, so only use for small number of regions with high ammounts of traffic
    // Needs dedicated thread running QueueAgent method
    // blocks until recieving requested memory
    MemRegion EnqueueAlloc(size_t itemSize, uint32_t regionID);

    // Enqueues request for allocator, lowering chance for starvation
    // All memory regions share one queue, so only use for small number of regions with high ammounts of traffic
    // Needs dedicated thread running QueueAgent method
    // non blocking after enqueuing. memory will be freed without callback.
    void EnqueueFree(MemRegion* memory, uint32_t regionID);

    // Infinite blocking loop making the request queue function
    // Launch thread using this function
    // Multiple threads may be created to serve requests
    void QueueAgent();

    // will make a random QueueAgent terminate
    void TerminateAgent();

protected:
    Allocator** m_allocators;
    uint32_t m_regions;
private:
    std::mutex* m_locks;

    void EnqueueRequest(AllocatorRequest* request);

    std::queue<AllocatorRequest> m_requestQueue;

    std::mutex m_dequeueCndLock;
    std::condition_variable m_dequeueCnd;

    std::mutex m_returnCndLock;
    std::condition_variable m_returnCnd;
};

class ThreadsafeStackAlloc : public ThreadsafeAllocator
{
public: // Methods
    ThreadsafeStackAlloc() = delete;
    //Will create one StackAlloc of size floor(totalMemory/regions) for each region requested
    ThreadsafeStackAlloc(uint32_t regions, size_t totalMemory = DEFAULT_MEM_SIZE);
    //The allocators and memory will be freed
    ~ThreadsafeStackAlloc();
private:
};

class ThreadsafePoolAlloc : public ThreadsafeAllocator
{
public: // Methods
    ThreadsafePoolAlloc() = delete;
    //Will create one PoolAlloc of size floor(totalMemory/regions) for each region requested
    ThreadsafePoolAlloc(uint32_t regions, size_t totalMemory = DEFAULT_MEM_SIZE, size_t nodeSize = DEFAULT_NODE_SIZE);
    //The allocators and memory will be freed
    ~ThreadsafePoolAlloc();
private:
};

class ThreadsafeMallocAlloc : public ThreadsafeAllocator
{
public: // Methods
    ThreadsafeMallocAlloc() = delete;
    //Will create one buddyAlloc of size floor(totalMemory/regions) for each region requested
    ThreadsafeMallocAlloc(uint32_t regions, size_t totalMemory = DEFAULT_MEM_SIZE);
    //The allocators and memory will be freed
    ~ThreadsafeMallocAlloc();
private:
};
