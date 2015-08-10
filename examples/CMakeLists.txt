cmake_minimum_required(VERSION 2.8.11)

project(qmqttExamples)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g3")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_executable(qmqttPublisher qmqttPublisher.cpp)
target_link_libraries(qmqttPublisher qmqtt)

add_executable(qmqttSubscriber qmqttSubscriber.cpp)
target_link_libraries(qmqttSubscriber qmqtt)
