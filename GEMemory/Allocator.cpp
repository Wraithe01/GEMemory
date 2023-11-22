#include "Allocator.h"

MemRegion::MemRegion(void* start, size_t elemSize)
{
    m_dataStart = static_cast<uint8_t*>(start);
    m_dataSize  = elemSize;
    m_dataEnd   = m_dataStart + elemSize;
}
MemRegion::~MemRegion() {}

bool MemRegion::IsValid() { return !(m_dataStart == nullptr); }

uint8_t* MemRegion::GetAt()
{
    assert(!(m_dataStart == nullptr) && "Invalid usage of memory region, region is freed.");
    return m_dataStart;
}
uint8_t* MemRegion::GetAtFree()
{
    assert(!(m_dataStart == nullptr) && "Invalid usage of memory region, region is freed.");
    uint8_t* graveptr = m_dataStart;
    m_dataStart       = nullptr;
    m_dataSize        = NULL;
    m_dataEnd         = nullptr;
    return graveptr;
}


const uint8_t* MemRegion::Read() const
{
    assert(!(m_dataStart == nullptr) && "Invalid usage of memory region, region is freed.");
    return m_dataStart;
}
const uint8_t* MemRegion::Read(int32_t index) const
{
    assert(!(m_dataStart == nullptr) && "Invalid usage of memory region, region is freed.");
    assert(!(abs(index) * m_dataSize >= (m_dataEnd - m_dataStart))
           && "Invalid memory access in Read(index).");

    if (index < 0)
        return (m_dataEnd - index * m_dataSize);
    return (m_dataStart + index * m_dataSize);
}

void MemRegion::Write(void* data, size_t dataSize)
{
    assert(!(m_dataStart == nullptr) && "Invalid usage of memory region, region is freed.");
    memcpy_s(m_dataStart, m_dataSize * (m_dataEnd - m_dataStart), data, dataSize);
}
void MemRegion::Write(uint32_t index, void* data, size_t dataSize)
{
    assert(!(m_dataStart == nullptr) && "Invalid usage of memory region, region is freed.");
    assert(!(index * m_dataSize >= (m_dataEnd - m_dataStart)) && "Invalid memory acess in Write()");
    memcpy_s((m_dataStart + (index * m_dataSize)),
             m_dataSize * (m_dataEnd - m_dataStart),
             data,
             dataSize);
}

Allocator::Allocator() { m_memory = new MemBlock(DEFAULT_MEM_SIZE); }
Allocator::Allocator(size_t size) { m_memory = new MemBlock(size); }

Allocator::~Allocator() { delete m_memory; }
