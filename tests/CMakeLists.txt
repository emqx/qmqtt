cmake_minimum_required(VERSION 3.9)

# Add auto tests (simple module tests)
add_subdirectory(auto)

# Add Google Test based unit tests
if(NOT WIN32 OR NOT NO_UNIT_TESTS)
    add_subdirectory(gtest)
endif()

# Add benchmarks (only in release mode)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_subdirectory(benchmarks)
endif()