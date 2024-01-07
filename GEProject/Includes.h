#pragma once
#include <cstdint>

// #include <raylib.h>  // Order is important
// #include <raymath.h>
#include "ResourceManager.h"
#include "MemoryAlloc.h"


#include <imgui.h>
#include "ImguiConfig.h"
#include "implot.h"

// Additional implot data 

// utility structure for realtime plot (from demo)
struct ScrollingBuffer
{
    int              MaxSize;
    int              Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000)
    {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y)
    {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else
        {
            Data[Offset] = ImVec2(x, y);
            Offset       = (Offset + 1) % MaxSize;
        }
    }
    void Erase()
    {
        if (Data.size() > 0)
        {
            Data.shrink(0);
            Offset = 0;
        }
    }
};

// For displaying dynamic amounts of allocators
struct ImAllocator
{
    Allocator* allocator;
    ScrollingBuffer buffer;
};