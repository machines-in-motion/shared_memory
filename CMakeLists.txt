cmake_minimum_required(VERSION 2.8.3)
project(shared_memory)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(cmake/pkg-config.cmake)
include(cmake/eigen.cmake)

find_package(catkin REQUIRED)
search_for_eigen()
find_package(Boost COMPONENTS thread system REQUIRED)

catkin_package(
    INCLUDE_DIRS include
    CATKIN_DEPENDS
    LIBRARIES shared_memory
    CFG_EXTRAS pkg-config.cmake eigen.cmake
)

include_directories(
    include
    ${Boost_INCLUDE_DIRS}
    ${Eigen_INCLUDE_DIRS}
)

###############
# add library #
###############

add_library(shared_memory 
  src/shared_memory.cpp
  src/exceptions.cpp
  src/simpleId.cpp)
target_link_libraries(shared_memory
  ${Boost_LIBRARIES}
)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries(shared_memory
  -lrt
  -pthread
)  
endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


#########
# demos #
#########

add_executable(set_data demos/set_data.cpp)
target_link_libraries(set_data shared_memory)

add_executable(get_data demos/get_data.cpp)
target_link_libraries(get_data shared_memory)

add_executable(cond_var_timeout demos/cond_var_timeout.cpp)
target_link_libraries(cond_var_timeout shared_memory)

add_executable(exchange_manager_clean
  demos/four_int_values.cpp
  demos/exchange_manager_clean.cpp)
target_link_libraries(exchange_manager_clean shared_memory)


add_executable(exchange_manager_producer
  demos/four_int_values.cpp
  demos/exchange_manager_producer.cpp)
target_link_libraries(exchange_manager_producer shared_memory)

add_executable(exchange_manager_consumer
  demos/four_int_values.cpp
  demos/exchange_manager_consumer.cpp)
target_link_libraries(exchange_manager_consumer shared_memory)

##############
# Unit tests #
##############

add_executable(tests_executable tests/support/tests_executable.cpp)
target_link_libraries(tests_executable shared_memory)
set_target_properties(tests_executable PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/")

catkin_add_gtest(shared_memory_unit_tests
  tests/main.cpp
  tests/unit_tests.cpp
  tests/unit_tests_serializable_stack.cpp
  tests/unit_tests_exchange_manager.cpp
  demos/four_int_values.cpp)
target_link_libraries(shared_memory_unit_tests shared_memory)
set_target_properties(shared_memory_unit_tests PROPERTIES
  COMPILE_DEFINITIONS
  SHM_PATH_TO_SUPPORT_EXE="${CMAKE_CURRENT_SOURCE_DIR}/tests/bin/tests_executable")


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

# simple executable the clean the benchmarks shared memory
add_executable(clean_shared_memory benchmarks/clean_shared_memory.cpp)
target_link_libraries(clean_shared_memory ${BENCHMARK_DEPENDENCIES})

###############################################
# Add custom (non compiling) targets          #
# so non cpp files show up in QT_CREATOR IDE. #
###############################################
set(USE_QTCREATOR_${PROJECT_NAME} OFF CACHE BOOL
    "Set to ON if you want QtCreator to display all files in the project tree")
if(USE_QTCREATOR_${PROJECT_NAME})
    file(GLOB_RECURSE EXTRA_FILES
        ../*.h ../*.hh ../*.hpp ../*hxx
        ../*.cf  ../*.yaml ../*.txt ../*.xml
        ../start ../*.launch ../*.urdf ../CATKIN_IGNORE ../*.in
    )
    list(FILTER EXTRA_FILES EXCLUDE REGEX "build")
    add_custom_target(${PROJECT_NAME}_OTHER_FILES ALL WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR} SOURCES ${EXTRA_FILES})
endif()
