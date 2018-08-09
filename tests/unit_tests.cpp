#include "shared_memory/shared_memory.hpp"
#include "shared_memory/tests/tests.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <unistd.h>
#include <sstream>

// set in the CMakeLists.txt file
// see tests/support/tests_executable.cpp
static std::string PATH_TO_EXECUTABLE = SHM_PATH_TO_SUPPORT_EXE;

static unsigned int TIME_SLEEP = 1000;  //microseconds

class Shared_memory_tests : public ::testing::Test {
protected:
  void SetUp() {}
  void TearDown() {
    shared_memory::clear();
  }
};


static bool _call_executable(int action){
  std::stringstream s;
  s << PATH_TO_EXECUTABLE << " " << action;
  s << std::ends;
  if(0 != std::system(s.str().c_str())){
    return false;
  }
  return true;
}


TEST_F(Shared_memory_tests,spawn_thread_test){
  bool called = _call_executable(shared_memory_test::set_double);
  ASSERT_EQ(called,true);
}


TEST_F(Shared_memory_tests,double_test){

  _call_executable(shared_memory_test::set_double);

  usleep(TIME_SLEEP);
  
  double value;
  shared_memory::get<double>(shared_memory_test::segment_id,
			     shared_memory_test::object_id,
			     value);
  ASSERT_EQ(value,shared_memory_test::test_double);
}


TEST_F(Shared_memory_tests,int_test){

  _call_executable(shared_memory_test::set_int);

  usleep(TIME_SLEEP);
  
  int value;
  shared_memory::get<int>(shared_memory_test::segment_id,
			     shared_memory_test::object_id,
			     value);
  ASSERT_EQ(value,shared_memory_test::test_int);
}


TEST_F(Shared_memory_tests,test_float){

  _call_executable(shared_memory_test::set_float);

  usleep(TIME_SLEEP);
  
  float value;
  shared_memory::get<float>(shared_memory_test::segment_id,
			     shared_memory_test::object_id,
			     value);
  ASSERT_EQ(value,shared_memory_test::test_float);
}


TEST_F(Shared_memory_tests,test_vector){

  _call_executable(shared_memory_test::set_vector);

  usleep(TIME_SLEEP);
  
  std::vector<double> v;
  shared_memory::get<double>(shared_memory_test::segment_id,
			     shared_memory_test::object_id,
			     v);

  ASSERT_EQ(v.size(),shared_memory_test::test_array_size);
  for(int i=0;i<shared_memory_test::test_array_size;i++){
    ASSERT_EQ(v[i],shared_memory_test::test_array[i]);
  }

}
