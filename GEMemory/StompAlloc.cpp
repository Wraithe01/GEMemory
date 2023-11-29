#include "StompAlloc.h"

StompAlloc::StompAlloc() : Allocator() {}

StompAlloc::StompAlloc(uint32_t memSize) : Allocator(memSize) {}

StompAlloc::~StompAlloc() {}

MemRegion StompAlloc::Alloc(size_t itemSize) {
	if (itemSize > PAGE_SIZE) {
		std::cout << "[-] Item bigger than pagesize\n";
		return MemRegion(nullptr, 0);
	}
	// Allocates two pages and sets the first to no access
	void* firstPage = VirtualAlloc(nullptr, PAGE_SIZE, MEM_COMMIT, PAGE_NOACCESS);
	void* secondPage = VirtualAlloc(static_cast<uint8_t*>(firstPage), PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);

	if (firstPage == nullptr || secondPage == nullptr)
		return MemRegion(nullptr, 0);

	// Sets memory allocation info
	MemoryAllocationInfo* info = reinterpret_cast<MemoryAllocationInfo*>(secondPage);
	info->size = itemSize;
	info->sentinel = SENTINEL_VALUE;

	// Get pointer to allocatable memory
	void* offset = reinterpret_cast<uint8_t*>(info) + sizeof(MemoryAllocationInfo);

	return MemRegion(offset, itemSize);
}

void StompAlloc::Free(MemRegion* memory) {
	if (memory == nullptr || !memory->IsValid()) {
		std::cout << "[-] Invalid memory in StompAlloc::Free\n";
		return;
	}

	// Get sentinel value from memory
	void* originalPointer = reinterpret_cast<uint8_t*>(memory->GetAt());
	MemoryAllocationInfo* info = reinterpret_cast<MemoryAllocationInfo*>(static_cast<uint8_t*>(originalPointer) - sizeof(MemoryAllocationInfo));

	// Sentinel check
	if (info->sentinel != SENTINEL_VALUE) {
		std::cout << "[-] Sentinel does not match, underrun detected.\n";
	}
	VirtualFree(info, 0, MEM_RELEASE);
}