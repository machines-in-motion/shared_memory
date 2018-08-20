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

add_library(shared_memory 
  src/shared_memory.cpp
  src/exceptions.cpp)
target_link_libraries(shared_memory ${Boost_LIBRARY_DIRS} -lrt -pthread)

add_executable(set_data src/examples/set_data.cpp)
target_link_libraries(set_data shared_memory)

add_executable(get_data src/examples/get_data.cpp)
target_link_libraries(get_data shared_memory)

add_executable(tests_executable tests/support/tests_executable.cpp)
target_link_libraries(tests_executable shared_memory)
install(TARGETS tests_executable RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tests/bin)
catkin_add_gtest(shared_memory_unit_tests
  tests/main.cpp
  tests/unit_tests.cpp)
target_link_libraries(shared_memory_unit_tests shared_memory)
set_target_properties(shared_memory_unit_tests PROPERTIES
  COMPILE_DEFINITIONS
  SHM_PATH_TO_SUPPORT_EXE="${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/tests_executable")



