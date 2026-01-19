cmake_minimum_required(VERSION 3.9)

# Create the benchmark executable
add_executable(tst_bench_message main.cpp)

# Link against required libraries
target_link_libraries(tst_bench_message
    PRIVATE
        qmqtt
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)

# Set properties
set_target_properties(tst_bench_message
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED OFF
        AUTOMOC ON
)

# Set release mode for benchmarks
target_compile_definitions(tst_bench_message
    PRIVATE
        QT_NO_DEBUG
)

# Add as test (benchmark)
add_test(NAME qmqtt_message_benchmark COMMAND tst_bench_message)