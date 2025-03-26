cmake_minimum_required(VERSION 3.10)
project(LRUCache LANGUAGES CXX)

# Set C++ standard and compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR})

# Main library target
add_library(lru_cache
    lru/lrucache.hpp
    lru/queue.hpp
)

# Interface target for users of the library
add_library(lru::cache ALIAS lru_cache)

# Testing configuration (GoogleTest)
if(BUILD_TESTS)
    enable_testing()
    
    # Find or download GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
    
    # Test executable
    add_executable(lru_cache_test
        test/lrucache_test.cpp
    )
    target_link_libraries(lru_cache_test
        PRIVATE
        lru_cache
        GTest::GTest
        GTest::Main
    )
    add_test(NAME lru_cache_test COMMAND lru_cache_test)
endif()

# Example program
if(BUILD_EXAMPLES)
    add_executable(lru_cache_example
        example.cpp
    )
    target_link_libraries(lru_cache_example
        PRIVATE
        lru_cache
    )
endif()

# Installation configuration
install(TARGETS lru_cache
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY lru/
    DESTINATION include/lru
    FILES_MATCHING PATTERN "*.hpp"
)