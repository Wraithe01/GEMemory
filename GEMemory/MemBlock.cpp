#include "MemBlock.h"

MemBlock::MemBlock()
{
    m_start = static_cast<uint8_t*>(malloc(DEFAULT_MEM_SIZE));
    assert(!(m_start == nullptr));
    m_end = m_start + (DEFAULT_MEM_SIZE * sizeof(uint8_t));
    memset(m_start, 0, DEFAULT_MEM_SIZE * sizeof(uint8_t));
}
MemBlock::MemBlock(size_t size)
{
    m_start = static_cast<uint8_t*>(malloc(size));
    assert(!(m_start == nullptr));
    m_end = m_start + (size * sizeof(uint8_t));
    memset(m_start, 0, size * sizeof(uint8_t));
}
MemBlock::~MemBlock() { free(m_start); }

void MemBlock::Resize(size_t newSize)
{
    void* temp = realloc(m_start, newSize);
    assert((m_start == nullptr) && "Resize resulted in nullptr. Aborting.");
    m_start = static_cast<uint8_t*>(temp);
    m_end   = m_start + (newSize * sizeof(uint8_t));
}

uint8_t* MemBlock::GetStart() const { return m_start; }

uint8_t* MemBlock::GetEnd() const { return m_end; }

size_t MemBlock::GetSize() const
{
    return static_cast<size_t>(m_end - m_start);
}
