#
# Copyright (c) 2019, New York University and Max Planck Gesellschaft.
#
# License BSD-3 clause
#

#
# set up the project
#
cmake_minimum_required(VERSION 3.10.2)

project(shared_memory)

# Using C++17
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

#
# Dependencies
#
# depend on ament macros
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

find_package(mpi_cmake_modules REQUIRED)
find_package(pybind11_vendor REQUIRED)
find_package(pybind11 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED)
find_package(cereal REQUIRED)
find_package(Threads REQUIRED)
find_package(rt REQUIRED)

# export de dependencies
ament_export_dependencies(Boost Threads Eigen3 cereal pybind11 rt)

# prepare to export all needed targets
set(all_targets)
set(all_target_exports)

#
# Add the main library
#
add_library(
  ${PROJECT_NAME} SHARED
  src/shared_memory.cpp
  src/locked_condition_variable.cpp
  src/condition_variable.cpp
  src/mutex.cpp
  src/lock.cpp
  src/exceptions.cpp
  src/array.cpp
  src/segment_info.cpp)
# Add the include dependencies
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)
# Link the dependencies
ament_target_dependencies(${PROJECT_NAME} Boost)
ament_target_dependencies(${PROJECT_NAME} cereal)
ament_target_dependencies(${PROJECT_NAME} rt)
ament_target_dependencies(${PROJECT_NAME} Eigen3)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

# Export the target.
ament_export_interfaces(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
list(APPEND all_targets ${PROJECT_NAME})
list(APPEND all_target_exports export_${PROJECT_NAME})

#
# Python wrapper
#
add_library(${PROJECT_NAME}_cpp_bindings MODULE srcpy/shared_memory.cpp)
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE pybind11::module)
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE ${PYTHON_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_cpp_bindings
                      PROPERTIES PREFIX "" SUFFIX "${PYTHON_MODULE_EXTENSION}")
target_include_directories(
  ${PROJECT_NAME}_cpp_bindings
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include> SYSTEM
  PUBLIC ${PYTHON_INCLUDE_DIRS})
_ament_cmake_python_get_python_install_dir()
install(TARGETS ${PROJECT_NAME}_cpp_bindings DESTINATION ${PYTHON_INSTALL_DIR})
install(FILES srcpy/__init__.py
        DESTINATION ${PYTHON_INSTALL_DIR}/${PROJECT_NAME})

#
# Unit tests
#
# add unit tests #
#
if(BUILD_TESTING)
  find_package(ament_cmake_gtest)

  # Set a general config folder path for all tests
  set(TEST_CONFIG_PATH ${PROJECT_SOURCE_DIR}/tests/config/)

  # Add an executable that populate the shared memory.
  add_executable(tests_executable tests/support/tests_executable.cpp
                                  demos/four_int_values.cpp)
  target_link_libraries(tests_executable shared_memory)
  set_target_properties(
    tests_executable PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                "${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/")

  # Add the test
  ament_add_gtest(test_shared_memory tests/main.cpp tests/unit_tests.cpp
                  demos/four_int_values.cpp)

  # Link the dependecies to it.
  target_link_libraries(test_shared_memory shared_memory)
  set_target_properties(
    test_shared_memory
    PROPERTIES
      COMPILE_DEFINITIONS
      SHM_PATH_TO_SUPPORT_EXE="${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/tests_executable"
  )
  add_dependencies(test_shared_memory tests_executable)
endif()

#
# demos
#
function(add_demo demo_name)
  # Deal with args
  set(options)
  set(oneValueArgs)
  set(multiValueArgs ADDITIONNAL_SOURCES)
  cmake_parse_arguments(add_demo "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  # set(demo_target_name ${PROJECT_NAME}_${demo_name})
  set(demo_target_name ${demo_name})
  # add the executable.
  add_executable(${demo_target_name} demos/${demo_name}.cpp
                                     ${add_demo_ADDITIONNAL_SOURCES})
  # save the name for installation
  set(all_targets ${all_targets} ${demo_target_name})
  # dependencies
  target_include_directories(
    ${demo_target_name}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)
  target_link_libraries(${demo_target_name} ${PROJECT_NAME})
  install(TARGETS ${demo_target_name} DESTINATION lib/${PROJECT_NAME})

endfunction(add_demo demo_name)

add_demo(set_data)
add_demo(get_data)
add_demo(cond_var_timeout)
add_demo(cond_var_demo)
add_demo(locked_cond_var_ping)
add_demo(locked_cond_var_pong)
add_demo(cond_var_ping)
add_demo(cond_var_pong)
add_demo(exchange_manager_producer ADDITIONNAL_SOURCES
         demos/four_int_values.cpp)
add_demo(exchange_manager_consumer ADDITIONNAL_SOURCES
         demos/four_int_values.cpp)
add_demo(exchange_manager_clean ADDITIONNAL_SOURCES demos/four_int_values.cpp)
add_demo(std_string_vector)
add_demo(serialization)
add_demo(demo_write_array)
add_demo(demo_read_array)
add_demo(demo_verbose)
add_demo(demo_eigen)
add_demo(non_existing_segment)
add_demo(create_segment)
add_demo(wait_for_segment)

#
# BENCHMARKS
#
function(add_benchmark benchmark_name)
  # Deal with args
  set(options)
  set(oneValueArgs)
  set(multiValueArgs ADDITIONNAL_SOURCES)
  cmake_parse_arguments(add_benchmark "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  # set(benchmark_target_name ${PROJECT_NAME}_${benchmark_name})
  set(benchmark_target_name ${benchmark_name})
  # add the executable.
  add_executable(${benchmark_target_name} benchmarks/${benchmark_name}.cpp
                                          ${add_benchmark_ADDITIONNAL_SOURCES})
  # save the name for installation
  set(all_targets ${all_targets} ${benchmark_target_name})
  # dependencies
  target_include_directories(
    ${benchmark_target_name}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)
  target_link_libraries(${benchmark_target_name} ${PROJECT_NAME})
endfunction(add_benchmark benchmark_name)

# benchmarks on the raw boost api
add_benchmark(stress_set_raw_boost_efficient)
add_benchmark(stress_get_raw_boost_efficient)
add_benchmark(stress_set_raw_boost_inefficient)
add_benchmark(stress_get_raw_boost_inefficient)

# benchmark using the current api
add_benchmark(stress_set_api)
add_benchmark(stress_get_api)

# simple executable to clean the benchmarks shared memory
add_benchmark(clean_shared_memory)

# not really a benchmark, just to get an idea how fast or slow serialization is
add_benchmark(serialization_frequency ADDITIONNAL_SOURCES
              demos/four_int_values.cpp)

#
# building documentation
#
add_documentation()

#
# Install the package
#
install(DIRECTORY include/ DESTINATION include)

install(
  TARGETS ${all_targets}
  EXPORT ${all_target_exports}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES
  DESTINATION include)

#
# Export the package as ament
#
ament_package()
