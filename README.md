# lru-cache-cpp
Modern C++ Implementation of Least Recently Used Cache

## Overview

An efficient LRU (Least Recently Used) cache library implemented in C++17, featuring:
- Fixed-capacity caching
- Least-recently-used eviction policy
- Optional TTL (Time-To-Live) expiration mechanism
- Custom eviction callback function

## Key Features

- 🚀 **High Performance**: O(1) time complexity for get/put operations
- ⏱ **TTL Support**: Automatic expiration of cache entries
- 💾 **Memory Safe**: Smart pointer resource management
- 📊 **Extensible**: Templated design supports arbitrary key-value types
- ✅ **Fully Tested**: Comprehensive GoogleTest unit tests

## Requirements
- C++17 compatible compiler (GCC 7+)
- CMake 3.10+
- GoogleTest (automatically downloaded, no manual installation needed)

## Quick Start

```cpp
#include "lru/lrucache.hpp"

// Create cache with capacity 3
LRUCache<int, std::string> cache(3);

// Add data
cache.put(1, std::make_unique<std::string>("Apple"));
cache.put(2, std::make_unique<std::string>("Banana"));

// Retrieve data
if (auto val = cache.get(1)) {
    std::cout << *val << std::endl; // Output: Apple
}

// Data with TTL (expires after 1000ms)
cache.put(3, std::make_unique<std::string>("Cherry"), 1000);
```
