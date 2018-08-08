#include "shared_memory/shared_memory.hpp"
#include "shared_memory/tests/tests.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <sstream>

// set in the CMakeLists.txt file
static std::string PATH_TO_EXECUTABLE = "SHM_PATH_TO_SUPPORT_EXE";

class Shared_memory_tests : public ::testing::Test {
protected:
  void SetUp() {}
  void TearDown() {}
};


bool _call_executable(int action){
  std::stringstream s;
  s << PATH_TO_EXECUTABLE << " " << action;
  s << std::ends;
  if(0 != std::system(s.str().c_str())){
    return false;
  }
  return true;
}


TEST_F(Shared_memory_tests,double_test){

  _call_executable(shared_memory_test::set_double);
  double value;
  shared_memory::get<double>(shared_memory_test::segment_id,
			     shared_memory_test::object_id,
			     value);
  ASSERT_EQ(value,shared_memory_test::test_double,value);
}

