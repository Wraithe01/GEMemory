#pragma once
#include "Allocator.h"
#include "Includes.h"


struct Node
{
    uint8_t* m_past;
    uint8_t* m_next;

    uint8_t* m_data;
};


// This is the pool allocator
class PoolAlloc : public Allocator
{
public:  // Methods
    PoolAlloc();
    ~PoolAlloc();

    virtual MemRegion& Alloc(void* item, size_t itemSize) override;
    virtual void             Free(void* start) override;

private:
public:  // variables
    Node m_start;
    Node m_last;

    uint32_t m_freeNodes;
    size_t   m_nodesize;
};
