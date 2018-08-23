cmake_minimum_required(VERSION 2.8.3)
project(shared_memory)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(cmake/pkg-config.cmake)
include(cmake/eigen.cmake)

find_package(catkin)
SEARCH_FOR_EIGEN()
find_package(Boost COMPONENTS thread system REQUIRED)

catkin_package(
    INCLUDE_DIRS include
    CATKIN_DEPENDS
    LIBRARIES shared_memory
)

include_directories(
    include
    ${Boost_INCLUDE_DIRS}
    ${Eigen_INCLUDE_DIRS}
)

add_library(shared_memory 
  src/shared_memory.cpp
  src/exceptions.cpp)
target_link_libraries(shared_memory ${Boost_LIBRARIES} -lrt -pthread)

add_executable(set_data src/examples/set_data.cpp)
target_link_libraries(set_data shared_memory)

add_executable(get_data src/examples/get_data.cpp)
target_link_libraries(get_data shared_memory)

add_executable(tests_executable tests/support/tests_executable.cpp)
target_link_libraries(tests_executable shared_memory)
set_target_properties(tests_executable PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/")

add_executable(stress_set_benchmark src/stress_tests/stress_set_1.cpp)
target_link_libraries(stress_set_benchmark ${Boost_LIBRARIES} -lrt -pthread)

add_executable(stress_set_api src/stress_tests/stress_set_2.cpp)
target_link_libraries(stress_set_api ${Boost_LIBRARIES} -lrt -pthread)

add_executable(stress_get_benchmark src/stress_tests/stress_get_1.cpp)
target_link_libraries(stress_get_benchmark ${Boost_LIBRARIES} -lrt -pthread)

add_executable(stress_get_api src/stress_tests/stress_get_2.cpp)
target_link_libraries(stress_get_api ${Boost_LIBRARIES} -lrt -pthread)


catkin_add_gtest(shared_memory_unit_tests
  tests/main.cpp
  tests/unit_tests.cpp)
target_link_libraries(shared_memory_unit_tests shared_memory)
set_target_properties(shared_memory_unit_tests PROPERTIES
  COMPILE_DEFINITIONS
  SHM_PATH_TO_SUPPORT_EXE="${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/tests_executable")



