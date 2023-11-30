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

uint32_t ThreadsafeAllocator::CurrentStored(uint32_t regionID)
{
	return m_allocators[regionID]->CurrentStored();
}

MemRegion ThreadsafeAllocator::EnqueueAlloc(size_t itemSize, uint32_t regionID)
{
	MemRegion memory(nullptr, 0);
	bool processed = false;
	AllocatorRequest request = {
		0,
		&memory,
		&processed,
		itemSize,
		regionID,
		MemRegion(nullptr, 0)
	};
	EnqueueRequest(&request);

	std::unique_lock<std::mutex> cndLock(m_returnCndLock);

	while (!processed) m_returnCnd.wait(cndLock);

	return memory;
}

void ThreadsafeAllocator::EnqueueFree(MemRegion* memory, uint32_t regionID)
{
	AllocatorRequest request = {
		1,
		nullptr,
		nullptr,
		0,
		regionID,
		*memory
	};
	EnqueueRequest(&request);
}

void ThreadsafeAllocator::QueueAgent()
{
	AllocatorRequest request = {
		-1,
		nullptr,
		nullptr,
		0,
		0,
		MemRegion(nullptr, 0)
	};
	std::unique_lock<std::mutex> cndLock(m_dequeueCndLock);
	while (true)
	{
		while (m_requestQueue.empty()) m_dequeueCnd.wait(cndLock);
		
		request = m_requestQueue.front();
		m_requestQueue.pop();

		cndLock.unlock();
		if (request.type == 0)
		{
			*(request.returnMemory) = Alloc(request.itemSize, request.regionID);
			*(request.processed) = true;
			m_returnCnd.notify_all();
		}
		else if (request.type == 1)
		{
			Free(&request.memory, request.regionID);
		}
		else if (request.type == 2)
		{
			break;
		}
		cndLock.lock();
	}
}

void ThreadsafeAllocator::TerminateAgent()
{
	AllocatorRequest request = {
		2,
		nullptr,
		nullptr,
		0,
		0,
		MemRegion(nullptr, 0)
	};
	EnqueueRequest(&request);
}

void ThreadsafeAllocator::EnqueueRequest(AllocatorRequest* request)
{
	// enqueues request
	m_dequeueCndLock.lock();
	m_requestQueue.push(*request);
	m_dequeueCndLock.unlock();
	// wakes queue agent if waiting
	m_dequeueCnd.notify_one();
}

ThreadsafeStackAlloc::ThreadsafeStackAlloc(uint32_t regions, size_t totalMemory) :
	ThreadsafeAllocator(regions, nullptr)
{
	size_t regionSize = (size_t)floor(totalMemory/regions);
	m_allocators = new Allocator*[m_regions];
	for (int32_t i = 0; i < m_regions; i++)
	{
		m_allocators[i] = new StackAlloc(regionSize);
	}
}

ThreadsafeStackAlloc::~ThreadsafeStackAlloc()
{
	for (int32_t i = 0; i < m_regions; i++)
	{
		delete m_allocators[i];
	}
	delete[] m_allocators;
}

ThreadsafePoolAlloc::ThreadsafePoolAlloc(uint32_t regions, size_t totalMemory, size_t nodeSize) : 
	ThreadsafeAllocator(regions, nullptr)
{
	size_t regionSize = (size_t)floor(totalMemory / regions);
	m_allocators = new Allocator * [m_regions];
	for (int32_t i = 0; i < m_regions; i++)
	{
		m_allocators[i] = new PoolAlloc(nodeSize, regionSize);
	}
}

ThreadsafePoolAlloc::~ThreadsafePoolAlloc()
{
	for (int32_t i = 0; i < m_regions; i++)
	{
		delete m_allocators[i];
	}
	delete[] m_allocators;
}

ThreadsafeMallocAlloc::ThreadsafeMallocAlloc(uint32_t regions, size_t totalMemory) :
	ThreadsafeAllocator(regions, nullptr)
{
	size_t regionSize = (size_t)floor(totalMemory / regions);
	m_allocators = new Allocator * [m_regions];
	for (int32_t i = 0; i < m_regions; i++)
	{
		m_allocators[i] = new MallocAlloc(regionSize);
	}
}

ThreadsafeMallocAlloc::~ThreadsafeMallocAlloc()
{
	for (int32_t i = 0; i < m_regions; i++)
	{
		delete m_allocators[i];
	}
	delete[] m_allocators;
}
