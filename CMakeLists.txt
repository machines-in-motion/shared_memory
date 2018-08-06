cmake_minimum_required(VERSION 2.8.3)
project(shared_memory)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

catkin_package(
    INCLUDE_DIRS include
    CATKIN_DEPENDS 
    LIBRARIES shared_memory
)

find_package(Boost COMPONENTS thread system REQUIRED)

include_directories(
    include
    ${Boost_INCLUDE_DIRS}
)

add_library(shared_memory SHARED src/shared_memory.cpp)
target_link_libraries(shared_memory ${Boost_LIBRARY_DIRS} -lrt -pthread)

add_executable(set_data src/tests/set_data.cpp)
target_link_libraries(set_data shared_memory)


add_executable(get_data src/tests/get_data.cpp)
target_link_libraries(get_data shared_memory)

		




