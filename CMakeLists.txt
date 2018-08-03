cmake_minimum_required(VERSION 2.8.3)
project(interprocesses_communication)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

catkin_package(
    INCLUDE_DIRS include
    CATKIN_DEPENDS 
    LIBRARIES 
)

find_package(Boost REQUIRED)

include_directories(
    include
    ${Boost_INCLUDE_DIRS}
)

add_executable(create_data src/tests/create_data.cpp)
target_link_libraries(create_data ${Boost_LIBRARIES})

add_executable(get_data src/tests/get_data.cpp)
target_link_libraries(get_data ${Boost_LIBRARIES})

		




