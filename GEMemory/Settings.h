#pragma once

// In bytes
constexpr auto DEFAULT_MEM_SIZE = 40'960;
// Data node in bytes
constexpr auto DEFAULT_NODE_SIZE = 64;

// Threads used in testing
constexpr auto TEST_NUM_THREADS = 2048;
// allocators used for thread tests
constexpr auto TEST_NUM_THREAD_REGIONS = 1;
// This cannot be equal to 0
constexpr auto POOL_OCCUPIED = 1;
