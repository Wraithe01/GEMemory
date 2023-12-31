#pragma once

#include <chrono>
#include "Scene.h"
#include "ResourceManager.h"
#include <windows.h> 

// To construct guid pattern
#include <iomanip>
#include <sstream>

class ResourceTest {

public:
    ResourceTest();
    ~ResourceTest();

    void Validate();

    void PerformanceBenchmark();
};