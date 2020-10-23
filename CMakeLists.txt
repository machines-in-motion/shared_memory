cmake_minimum_required(VERSION 2.8.3)
project(shared_memory)

# required to use std::shared_ptr in code and to link the python bindings
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed")
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

find_package(catkin REQUIRED COMPONENTS mpi_cmake_modules pybind11_catkin)
search_for_eigen()
search_for_boost()
search_for_cereal_required()

include_directories(
    ${catkin_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${Eigen_INCLUDE_DIRS}
    ${cereal_INCLUDE_DIRS}
    include
)

###################################
# Declare the libraries to export #
###################################

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(NON_DARWIN_LIBRARIES rt pthread)
endif()

catkin_package(
    INCLUDE_DIRS include
    CATKIN_DEPENDS
    LIBRARIES shared_memory ${protobuf_cmake_target} ${NON_DARWIN_LIBRARIES}
)

###############
# add library #
###############

add_library(shared_memory
  src/shared_memory.cpp
  src/locked_condition_variable.cpp
  src/condition_variable.cpp
  src/mutex.cpp
  src/lock.cpp
  src/exceptions.cpp
  src/array.cpp
  src/segment_info.cpp)
target_link_libraries(shared_memory
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
  ${NON_DARWIN_LIBRARIES}
)

#########
# demos #
#########

add_executable(set_data demos/set_data.cpp)
target_link_libraries(set_data shared_memory ${protobuf_cmake_target})

add_executable(get_data demos/get_data.cpp)
target_link_libraries(get_data shared_memory ${protobuf_cmake_target})

add_executable(cond_var_timeout demos/cond_var_timeout.cpp)
target_link_libraries(cond_var_timeout shared_memory)

add_executable(cond_var_demo demos/cond_var_demo.cpp)
target_link_libraries(cond_var_demo shared_memory)

add_executable(locked_cond_var_ping demos/locked_cond_var_ping.cpp)
target_link_libraries(locked_cond_var_ping shared_memory)

add_executable(locked_cond_var_pong demos/locked_cond_var_pong.cpp)
target_link_libraries(locked_cond_var_pong shared_memory)

add_executable(cond_var_ping demos/cond_var_ping.cpp)
target_link_libraries(cond_var_ping shared_memory)

add_executable(cond_var_pong demos/cond_var_pong.cpp)
target_link_libraries(cond_var_pong shared_memory)

add_executable(exchange_manager_producer
  demos/four_int_values.cpp
  demos/exchange_manager_producer.cpp)
target_link_libraries(exchange_manager_producer shared_memory)

add_executable(exchange_manager_consumer
  demos/four_int_values.cpp
  demos/exchange_manager_consumer.cpp)
target_link_libraries(exchange_manager_consumer shared_memory)

add_executable(exchange_manager_clean
  demos/four_int_values.cpp
  demos/exchange_manager_clean.cpp)
target_link_libraries(exchange_manager_clean shared_memory)

add_executable(std_string_vector
  demos/std_string_vector.cpp)
target_link_libraries(std_string_vector shared_memory)

add_executable(serialization
  demos/serialization.cpp)
target_link_libraries(serialization shared_memory)

add_executable(demo_write_array
  demos/demo_write_array.cpp)
target_link_libraries(demo_write_array shared_memory)

add_executable(demo_read_array
  demos/demo_read_array.cpp)
target_link_libraries(demo_read_array shared_memory)

add_executable(demo_verbose
  demos/demo_verbose.cpp)
target_link_libraries(demo_verbose shared_memory)

add_executable(demo_eigen
  demos/demo_eigen.cpp)
target_link_libraries(demo_eigen shared_memory)

add_executable(non_existing_segment demos/non_existing_segment.cpp)
target_link_libraries(non_existing_segment shared_memory)

add_executable(create_segment demos/create_segment.cpp)
target_link_libraries(create_segment shared_memory)

add_executable(wait_for_segment demos/wait_for_segment.cpp)
target_link_libraries(wait_for_segment shared_memory)

##################
# Python wrapper #
##################

pybind11_add_module(shared_memory_py srcpy/wrappers.cpp)
target_link_libraries(shared_memory_py PRIVATE shared_memory ${catkin_LIBRARIES})
set_target_properties(shared_memory_py PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CATKIN_DEVEL_PREFIX}/${CATKIN_GLOBAL_PYTHON_DESTINATION}
  OUTPUT_NAME shared_memory)
install(TARGETS shared_memory_py DESTINATION ${CATKIN_GLOBAL_PYTHON_DESTINATION})

##############
# Unit tests #
##############

add_executable(tests_executable
  tests/support/tests_executable.cpp
  demos/four_int_values.cpp)
target_link_libraries(tests_executable shared_memory)
set_target_properties(tests_executable PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/")

catkin_add_gtest(shared_memory_unit_tests
  tests/main.cpp
  tests/unit_tests.cpp
  demos/four_int_values.cpp)

if (TARGET shared_memory_unit_tests)

    target_link_libraries(shared_memory_unit_tests shared_memory)
    set_target_properties(shared_memory_unit_tests PROPERTIES
                          COMPILE_DEFINITIONS
                          SHM_PATH_TO_SUPPORT_EXE="${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/tests_executable")
    add_dependencies(shared_memory_unit_tests tests_executable)

endif()

##############
# BENCHMARKS #
##############

set(BENCHMARK_DEPENDENCIES
  ${Boost_LIBRARIES}
)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(BENCHMARK_DEPENDENCIES
  ${BENCHMARK_DEPENDENCIES}
  -lrt
  -pthread
)
endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#find_package(benchmark REQUIRED)

# benchmarks on the raw boost api
add_executable(stress_set_raw_boost_efficient
#  benchmarks/main.cpp
  benchmarks/stress_set_raw_boost_efficient.cpp)
target_link_libraries(stress_set_raw_boost_efficient ${BENCHMARK_DEPENDENCIES})

add_executable(stress_get_raw_boost_efficient
#  benchmarks/main.cpp
  benchmarks/stress_get_raw_boost_efficient.cpp)
target_link_libraries(stress_get_raw_boost_efficient ${BENCHMARK_DEPENDENCIES})

add_executable(stress_set_raw_boost_inefficient
#  benchmarks/main.cpp
  benchmarks/stress_set_raw_boost_inefficient.cpp)
target_link_libraries(stress_set_raw_boost_inefficient ${BENCHMARK_DEPENDENCIES})

add_executable(stress_get_raw_boost_inefficient
#  benchmarks/main.cpp
  benchmarks/stress_get_raw_boost_inefficient.cpp)
target_link_libraries(stress_get_raw_boost_inefficient ${BENCHMARK_DEPENDENCIES})

# benchmark using the current api
add_executable(stress_set_api benchmarks/stress_set_api.cpp)
target_link_libraries(stress_set_api shared_memory ${BENCHMARK_DEPENDENCIES})

add_executable(stress_get_api benchmarks/stress_get_api.cpp)
target_link_libraries(stress_get_api
  shared_memory ${Boost_LIBRARIES} ${BENCHMARK_DEPENDENCIES})

# simple executable to clean the benchmarks shared memory
add_executable(clean_shared_memory benchmarks/clean_shared_memory.cpp)
target_link_libraries(clean_shared_memory ${BENCHMARK_DEPENDENCIES})

# not really a benchmark, just to get an idea how fast or slow
# serialization is
add_executable(serialization_frequency
  demos/four_int_values.cpp
  benchmarks/serialization_frequency.cpp)
target_link_libraries(serialization_frequency ${BENCHMARK_DEPENDENCIES})



##########################
# building documentation #
##########################
build_doxygen_documentation()

#####################################################################
# Format the code upon build with the cmake option -DFORMAT_CODE=ON #
#####################################################################
format_code()
