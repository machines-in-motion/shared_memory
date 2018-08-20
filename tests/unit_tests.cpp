#include "shared_memory/shared_memory.hpp"
#include "shared_memory/tests/tests.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <unistd.h>
#include <sstream>

// set in the CMakeLists.txt file
// see tests/support/tests_executable.cpp
static std::string PATH_TO_EXECUTABLE = SHM_PATH_TO_SUPPORT_EXE;

static unsigned int TIME_SLEEP = 5000;  //microseconds

class Shared_memory_tests : public ::testing::Test {
protected:
  void SetUp() {
    shared_memory::clear_mutex(shared_memory_test::object_id);
    shared_memory::clear_segment(shared_memory_test::segment_id);
  }
  void TearDown() {
    shared_memory::clear_mutex(shared_memory_test::object_id);
    shared_memory::clear_segment(shared_memory_test::segment_id);
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


TEST_F(Shared_memory_tests,test_string){

  _call_executable(shared_memory_test::set_string);

  usleep(TIME_SLEEP);

  std::string str;
  shared_memory::get<std::string>(shared_memory_test::segment_id,
				  shared_memory_test::object_id,
				  str);
  ASSERT_EQ(str,shared_memory_test::test_string);
  
}


TEST_F(Shared_memory_tests,test_array){

  _call_executable(shared_memory_test::set_double_array);

  usleep(TIME_SLEEP);
  
  double a[shared_memory_test::test_array_size];
  shared_memory::get<double>(shared_memory_test::segment_id,
			     shared_memory_test::object_id,
			     a,
			     shared_memory_test::test_array_size);

  for(int i=0;i<shared_memory_test::test_array_size;i++){
    ASSERT_EQ(a[i],shared_memory_test::test_array[i]);
  }

}



TEST_F(Shared_memory_tests,test_vector){

  _call_executable(shared_memory_test::set_vector);

  usleep(TIME_SLEEP);

  std::vector<double> v(shared_memory_test::test_array_size);
  shared_memory::get<double>(shared_memory_test::segment_id,
			     shared_memory_test::object_id,
			     v);

  ASSERT_EQ(v.size(),shared_memory_test::test_array_size);
  for(int i=0;i<shared_memory_test::test_array_size;i++){
    ASSERT_EQ(v[i],shared_memory_test::test_array[i]);
  }

}


TEST_F(Shared_memory_tests,test_int_double_map){

  _call_executable(shared_memory_test::set_int_double_map);

  usleep(TIME_SLEEP);

  std::map<int,double> m;
  m[shared_memory_test::map_int_keys1]=0.0;
  m[shared_memory_test::map_int_keys2]=0.0;
  shared_memory::get<int,double>(shared_memory_test::segment_id,
				 shared_memory_test::object_id,
				 m);

  ASSERT_EQ(m.size(),shared_memory_test::test_map_size);
  ASSERT_EQ(m[shared_memory_test::map_int_keys1],shared_memory_test::map_value_1);
  ASSERT_EQ(m[shared_memory_test::map_int_keys2],shared_memory_test::map_value_2);

}


TEST_F(Shared_memory_tests,test_string_double_map){

  _call_executable(shared_memory_test::set_string_double_map);

  usleep(TIME_SLEEP);

  std::map<std::string,double> m;
  m[shared_memory_test::map_string_keys1]=0.0;
  m[shared_memory_test::map_string_keys2]=0.0;
  shared_memory::get<std::string,double>(shared_memory_test::segment_id,
				 shared_memory_test::object_id,
				 m);

  ASSERT_EQ(m.size(),shared_memory_test::test_map_size);
  ASSERT_EQ(m[shared_memory_test::map_string_keys1],shared_memory_test::map_value_1);
  ASSERT_EQ(m[shared_memory_test::map_string_keys2],shared_memory_test::map_value_2);

}


TEST_F(Shared_memory_tests,test_memory_overflow){

  unsigned int max_size = SHARED_MEMORY_SIZE_ / sizeof(int) + 1 ;
  std::vector<int> v(max_size);
  for(int i=0;i<v.size();i++) v[i]=1;

  ASSERT_THROW (
	       shared_memory::set<int>(shared_memory_test::segment_id,
				       shared_memory_test::object_id,
				       v) ,
	       shared_memory::Allocation_exception);

}

TEST_F(Shared_memory_tests,test_wrong_size_vector){

  _call_executable(shared_memory_test::set_vector);

  usleep(TIME_SLEEP);
  
  std::vector<double> v(shared_memory_test::test_array_size + 1); // !

  ASSERT_THROW (
		shared_memory::get<double>(shared_memory_test::segment_id,
					   shared_memory_test::object_id,
					   v) ,
		shared_memory::Unexpected_size_exception);

}
