cmake_minimum_required(VERSION 2.8.11)

project(qmqtt)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-O0 -g0")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

file(GLOB_RECURSE srcfiles "*.cpp")
file(GLOB_RECURSE headerfiles "*.h")

add_library(qmqtt STATIC ${headerfiles} ${srcfiles})

qt5_use_modules(qmqtt Core Network)

add_subdirectory( examples )
