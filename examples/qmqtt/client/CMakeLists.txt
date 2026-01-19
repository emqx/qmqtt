cmake_minimum_required(VERSION 3.9)

# Create the example executable
add_executable(qmqtt_example example.cpp)

# Link against the qmqtt library
target_link_libraries(qmqtt_example 
    PRIVATE 
        qmqtt
        Qt${QT_VERSION_MAJOR}::Core
)

# Set C++ standard to match the library
set_target_properties(qmqtt_example
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED OFF
)

# Enable Qt MOC for this target
set_target_properties(qmqtt_example PROPERTIES AUTOMOC ON)

# Optional: Install the example
install(TARGETS qmqtt_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
    COMPONENT Examples
)