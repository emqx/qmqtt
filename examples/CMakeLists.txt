project(qmqttExamples)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g3")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(../)
link_directories(../)

add_executable(qmqttClient qmqttClient.cpp)
target_link_libraries(qmqttClient qmqtt)
qt5_use_modules(qmqttClient Core Network)

add_executable(qmqttPublisher qmqttPublisher.cpp)
target_link_libraries(qmqttPublisher qmqtt)
qt5_use_modules(qmqttPublisher Core Network)
target_compile_features(qmqttPublisher PRIVATE cxx_range_for)

add_executable(qmqttSubscriber qmqttSubscriber.cpp)
target_link_libraries(qmqttSubscriber qmqtt)
qt5_use_modules(qmqttSubscriber Core Network)
target_compile_features(qmqttSubscriber PRIVATE cxx_range_for)
