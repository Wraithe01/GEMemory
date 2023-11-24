#include "ThreadSafeAllocators.h"

ThreadsafeAllocator::ThreadsafeAllocator(uint32_t regions, Allocator** regionAllocators):
	m_allocators(regionAllocators),
	m_regions(regions)
{
	m_locks = new std::mutex[m_regions];
}

ThreadsafeAllocator::~ThreadsafeAllocator()
{
	delete[] m_locks;
}

MemRegion ThreadsafeAllocator::Alloc(size_t itemSize, uint32_t regionID)
{
	if(regionID >= m_regions) return MemRegion(nullptr, 0);
	m_locks[regionID].lock();
	MemRegion mem = m_allocators[regionID]->Alloc(itemSize);
	m_locks[regionID].unlock();
	return mem;
}

void ThreadsafeAllocator::Free(MemRegion* memory, uint32_t regionID)
{
	if (regionID >= m_regions) return;
	m_locks[regionID].lock();
	m_allocators[regionID]->Free(memory);
	m_locks[regionID].unlock();
}

ThreadsafeStackAlloc::ThreadsafeStackAlloc(uint32_t regions, size_t totalMemory) :
	ThreadsafeAllocator(regions, nullptr)
{
	size_t regionSize = (size_t)floor(totalMemory/regions);
	m_allocators = new Allocator*[m_regions];
	
}

ThreadsafeStackAlloc::~ThreadsafeStackAlloc()
{
}
