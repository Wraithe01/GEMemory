#pragma once
#include "Includes.h"
#include "MemBlock.h"

// This is the return value of all allocate functions, a security layer.
class MemRegion
{
public:
    MemRegion() = delete;
    MemRegion(void* start, size_t elemSize);
    ~MemRegion();

    // Returns wether memory allocation was sucessful or not
    bool IsValid();

    // Returns the start pointer of the allocated memory region (potentially unsafe)
    uint8_t* GetAt();
    // Returns the start pointer of the allocated memory region-
    // and then deletes the memory region from further use.
    uint8_t* GetAtFree();

    // After reading, cast to appropriate type. Returns a pointer to the memory at given location.
    const uint8_t* Read() const;
    // After reading, cast to appropriate type. Returns a pointer to the memory at given location.
    const uint8_t* Read(int32_t index) const;

    // Writes data to the first index of the memory region
    void Write(void* data, size_t dataSize);
    // Writes data to the given index
    void Write(uint32_t index, void* data, size_t dataSize);

private:
    uint8_t* m_dataStart;
    size_t   m_dataSize;
    uint8_t* m_dataEnd;
};

// This is the base allocator class, if you are going to implement a new allocator, it should derive
// from this one.
class Allocator
{
public:  // Methods
    Allocator();
    Allocator(size_t size);
    ~Allocator();

    [[nodiscard("Dropping MemRegion might result in memory leak.")]]
    virtual MemRegion Alloc(size_t itemSize) = 0;

    virtual void Free(MemRegion* memory) = 0;

    // Returns the total size of stored data
    virtual size_t CurrentStored();

private:
protected:  // variables
    MemBlock* m_memory;
};
