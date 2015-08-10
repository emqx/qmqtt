cmake_minimum_required(VERSION 2.8.11)

project(qmqtt)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-O0 -g0")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

file(GLOB srcfiles "*.cpp")
file(GLOB headerfiles "*.h")

add_library(qmqtt SHARED ${srcfiles} ${headerfiles})
target_link_libraries(qmqtt Qt5::Core Qt5::Network)

install(TARGETS qmqtt DESTINATION "bin")
install(FILES ${headerfiles} DESTINATION "include")

add_subdirectory(examples)
