#pragma once
#include "Includes.h"

// This is what allocates the memory blocks and allows us to not use malloc and free.
class MemBlock
{
public:  // Methods
    MemBlock();
    MemBlock(size_t size);
    ~MemBlock();

    // Resizes the memory block and moves all current data with it.
    void Resize(size_t newSize);

    // Returns the begining of the memory block.
    uint8_t* GetStart() const;
    // Returns the end of the memory block.
    uint8_t* GetEnd() const;

    size_t GetSize() const;

private:
private:  // variables
    uint8_t* m_start;
    uint8_t* m_end;
};
