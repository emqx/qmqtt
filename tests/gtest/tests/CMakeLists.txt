cmake_minimum_required(VERSION 3.9)

# Find Qt Test component
find_package(Qt5 COMPONENTS Test REQUIRED)

# Create the test executable
add_executable(qmqtt_tests
    main.cpp
    clienttest.cpp
    tcpserver.cpp
    customprinter.cpp
    networktest.cpp
    messagetest.cpp
    frametest.cpp
    sockettest.cpp
    # Add private implementation files needed by tests
    ${CMAKE_SOURCE_DIR}/src/mqtt/qmqtt_network.cpp
    ${CMAKE_SOURCE_DIR}/src/mqtt/qmqtt_socket.cpp
    ${CMAKE_SOURCE_DIR}/src/mqtt/qmqtt_timer.cpp
    # Note: Removed Qt Test files that conflict with Google Test:
    # routedmessagetests.cpp - uses QTEST_MAIN
    # routertests.cpp - uses QTEST_MAIN  
    # routesubscriptiontests.cpp - uses QTEST_MAIN
)

# Link against required libraries
target_link_libraries(qmqtt_tests
    PRIVATE
        qmqtt
        Qt5::Core
        Qt5::Network
        Qt5::Test
        gtest
        gmock
)

# Include directories
target_include_directories(qmqtt_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../gtest/googletest/googletest/include
        ../gtest/googletest/googlemock/include
        ${CMAKE_SOURCE_DIR}/src/mqtt  # Access to private headers
)

# Compile definitions
target_compile_definitions(qmqtt_tests
    PRIVATE
        QMQTT_LIBRARY_TESTS
        QT_NO_SSL
)

# Set C++ standard
set_target_properties(qmqtt_tests
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED OFF
        AUTOMOC ON
)

# Copy qmqtt.dll to test directory for Windows
if(WIN32)
    add_custom_command(TARGET qmqtt_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:qmqtt>
        $<TARGET_FILE_DIR:qmqtt_tests>
    )
endif()

# Add as test with proper environment
add_test(NAME qmqtt_unit_tests COMMAND qmqtt_tests)

# Set test environment for Qt DLLs on Windows
if(WIN32)
    set_tests_properties(qmqtt_unit_tests PROPERTIES
        ENVIRONMENT "PATH=${CMAKE_PREFIX_PATH}/bin;$ENV{PATH}"
    )
endif()