#include "StompAlloc.h"

StompAlloc::StompAlloc() : Allocator() {}

StompAlloc::StompAlloc(uint32_t memSize) : Allocator(memSize) {}

StompAlloc::~StompAlloc() {}

MemRegion StompAlloc::Alloc(size_t itemSize) {

	void* firstPage = VirtualAlloc(nullptr, PAGE_SIZE, MEM_RESERVE, PAGE_NOACCESS);
	void* secondPage = VirtualAlloc(static_cast<uint8_t*>(firstPage), PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);

	MemoryAllocationInfo* info = reinterpret_cast<MemoryAllocationInfo*>(secondPage);
	info->size = itemSize;
	info->sentinel = SENTINEL_VALUE;

	void* offset = info + sizeof(MemoryAllocationInfo);

	return MemRegion(offset, itemSize);
}

void StompAlloc::Free(MemRegion* memory) {

	void* originalPointer = reinterpret_cast<uint8_t*>(memory->GetAt());

    MemoryAllocationInfo* info = reinterpret_cast<MemoryAllocationInfo*>(static_cast<uint8_t*>(originalPointer) - sizeof(MemoryAllocationInfo));

    // Check the sentinel value before freeing the memory
    if (info->sentinel != SENTINEL_VALUE) {
		std::cout << info->sentinel;
		std::cout << "Sentinel does not match, underrun detected !!! WARNING WARNING WARNING !!!";
    }
	VirtualFree(info, 0, MEM_RELEASE);
}
