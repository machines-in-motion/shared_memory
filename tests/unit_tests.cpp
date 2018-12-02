#include "shared_memory/shared_memory.hpp"
#include "shared_memory/thread_synchronisation.hpp"
#include "shared_memory/tests/tests.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <unistd.h>
#include <sstream>

// set in the CMakeLists.txt file
// see tests/support/tests_executable.cpp
static std::string PATH_TO_EXECUTABLE = SHM_PATH_TO_SUPPORT_EXE;

static unsigned int TIME_SLEEP = 5000;  //microseconds


static inline void clear_memory(){
  shared_memory::clear_shared_memory(shared_memory_test::segment_id);
}

class Shared_memory_tests : public ::testing::Test {
protected:
  void SetUp() {
    clear_memory();
    shared_memory::get_segment_mutex(shared_memory_test::segment_id).unlock();
  }
  void TearDown() {
    clear_memory();
  }
};

class DISABLED_Shared_memory_tests: public ::testing::Test
{};

static bool _call_executable(int action,bool blocking=false);

static bool _call_executable(int action,bool blocking){
  std::stringstream s;
  s << PATH_TO_EXECUTABLE << " " << action;
  if(!blocking){
    s << " &";
  }
  s << std::ends;
  if(blocking){
    if(0 != std::system(s.str().c_str())){
      return false;
    }
    return true;
  }
  std::system(s.str().c_str());
}


TEST_F(Shared_memory_tests,spawn_thread_test){
  bool called = _call_executable(shared_memory_test::set_double,true);
  ASSERT_EQ(called,true);
}


TEST_F(Shared_memory_tests,double_test){

  _call_executable(shared_memory_test::set_double);

  usleep(TIME_SLEEP);
  
  double value;
  shared_memory::get(shared_memory_test::segment_id,
           shared_memory_test::object_id,
           value);
  ASSERT_EQ(value,shared_memory_test::test_double);

}


TEST_F(Shared_memory_tests,int_test){

  _call_executable(shared_memory_test::set_int);

  usleep(TIME_SLEEP);
  
  int value;
  shared_memory::get(shared_memory_test::segment_id,
           shared_memory_test::object_id,
           value);
  ASSERT_EQ(value,shared_memory_test::test_int);

}


TEST_F(Shared_memory_tests,test_float){

  _call_executable(shared_memory_test::set_float);

  usleep(TIME_SLEEP);
  
  float value;
  shared_memory::get(shared_memory_test::segment_id,
           shared_memory_test::object_id,
           value);
  ASSERT_EQ(value,shared_memory_test::test_float);

}


TEST_F(Shared_memory_tests,test_string){

  _call_executable(shared_memory_test::set_string);

  usleep(TIME_SLEEP);

  std::string str;
  shared_memory::get(shared_memory_test::segment_id,
         shared_memory_test::object_id,
         str);
  ASSERT_EQ(str,shared_memory_test::test_string);
  
}


TEST_F(Shared_memory_tests,test_array){

  _call_executable(shared_memory_test::set_double_array);

  usleep(TIME_SLEEP);
  
  double a[shared_memory_test::test_array_size];
  shared_memory::get(shared_memory_test::segment_id,
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
  shared_memory::get(shared_memory_test::segment_id,
         shared_memory_test::object_id,
         v);

  ASSERT_EQ(v.size(),shared_memory_test::test_array_size);
  for(int i=0;i<shared_memory_test::test_array_size;i++){
    ASSERT_EQ(v[i],shared_memory_test::test_array[i]);
  }

}

TEST_F(Shared_memory_tests,test_eigen_vector){

  _call_executable(shared_memory_test::set_vector);

  usleep(TIME_SLEEP);

  Eigen::VectorXd v(shared_memory_test::test_array_size);
  shared_memory::get(shared_memory_test::segment_id,
         shared_memory_test::object_id,
         v);

  ASSERT_EQ(v.size(),shared_memory_test::test_array_size);
  for(int i=0;i<shared_memory_test::test_array_size;i++){
    ASSERT_EQ(v(i),shared_memory_test::test_array[i]);
  }

}

TEST_F(Shared_memory_tests,test_int_double_map){

  _call_executable(shared_memory_test::set_int_double_map);

  usleep(TIME_SLEEP);

  std::map<int,double> m;
  m[shared_memory_test::map_int_keys1]=0.0;
  m[shared_memory_test::map_int_keys2]=0.0;
  shared_memory::get(shared_memory_test::segment_id,
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
  shared_memory::get(shared_memory_test::segment_id,
         shared_memory_test::object_id,
         m);

  ASSERT_EQ(m.size(),shared_memory_test::test_map_size);
  ASSERT_EQ(m[shared_memory_test::map_string_keys1],shared_memory_test::map_value_1);
  ASSERT_EQ(m[shared_memory_test::map_string_keys2],shared_memory_test::map_value_2);

}


TEST_F(Shared_memory_tests,test_string_vector_double_map){

  _call_executable(shared_memory_test::set_string_vector_double_map);

  usleep(TIME_SLEEP);

  std::map<std::string,std::vector<double>> m;

  std::vector<double> v1(2);
  v1[0]=0.0;
  v1[1]=0.0;

  std::vector<double> v2(2);
  v2[0]=0.0;
  v2[1]=0.0;
  
  m[shared_memory_test::map_string_keys1]=v1;
  m[shared_memory_test::map_string_keys2]=v2;

  shared_memory::get(shared_memory_test::segment_id,
         shared_memory_test::object_id,
         m);

  ASSERT_EQ(m.size(),shared_memory_test::test_map_size);
  ASSERT_EQ(m[shared_memory_test::map_string_keys1].size(),2);
  ASSERT_EQ(m[shared_memory_test::map_string_keys2].size(),2);

  ASSERT_EQ(m[shared_memory_test::map_string_keys1][0],shared_memory_test::map_value_1);
  ASSERT_EQ(m[shared_memory_test::map_string_keys1][1],shared_memory_test::map_value_2);

  ASSERT_EQ(m[shared_memory_test::map_string_keys2][0],shared_memory_test::map_value_2);
  ASSERT_EQ(m[shared_memory_test::map_string_keys2][1],shared_memory_test::map_value_1);
  
}

TEST_F(Shared_memory_tests,test_string_vector_eigen_map){

  _call_executable(shared_memory_test::set_string_vector_eigen_map);

  usleep(TIME_SLEEP);

  std::map<std::string,Eigen::VectorXd> m;

  Eigen::VectorXd v1(2), v2(2);

  m[shared_memory_test::map_string_keys1]=v1;
  m[shared_memory_test::map_string_keys2]=v2;

  shared_memory::get(shared_memory_test::segment_id,
         shared_memory_test::object_id,
         m);

  ASSERT_EQ(m.size(),shared_memory_test::test_map_size);
  ASSERT_EQ(m[shared_memory_test::map_string_keys1].size(),2);
  ASSERT_EQ(m[shared_memory_test::map_string_keys2].size(),2);

  ASSERT_EQ(m[shared_memory_test::map_string_keys1][0],shared_memory_test::map_value_1);
  ASSERT_EQ(m[shared_memory_test::map_string_keys1][1],shared_memory_test::map_value_2);

  ASSERT_EQ(m[shared_memory_test::map_string_keys2][0],shared_memory_test::map_value_2);
  ASSERT_EQ(m[shared_memory_test::map_string_keys2][1],shared_memory_test::map_value_1);
}

TEST_F(Shared_memory_tests,test_memory_overflow){

  unsigned int max_size = SHARED_MEMORY_SIZE / sizeof(int) + 1 ;
  std::vector<int> v(max_size);
  for(int i=0;i<v.size();i++) v[i]=1;

  ASSERT_THROW (
         shared_memory::set(shared_memory_test::segment_id,
          shared_memory_test::object_id,
          v) ,
         shared_memory::Allocation_exception);

}


TEST_F(Shared_memory_tests,test_wrong_size_vector){

  _call_executable(shared_memory_test::set_vector);

  usleep(TIME_SLEEP);
  
  std::vector<double> v(shared_memory_test::test_array_size + 1); // !

  ASSERT_THROW (
    shared_memory::get(shared_memory_test::segment_id,
           shared_memory_test::object_id,
           v) ,
    shared_memory::Unexpected_size_exception);

}


static inline bool is_one_of(double v, double a1, double a2){
  if(v==a1) return true;
  if(v==a2) return true;
  return false;
}

TEST_F(Shared_memory_tests,test_concurrency){

  _call_executable(shared_memory_test::concurrent_1);
  _call_executable(shared_memory_test::concurrent_2);

  usleep(TIME_SLEEP);

  bool set_1_observed = false;
  bool set_2_observed = false;

  int nb_1 = 0;
  int nb_2 = 0;
  
  double a[shared_memory_test::test_array_size];
  
  while (true){

    shared_memory::get(shared_memory_test::segment_id,
           shared_memory_test::object_id,
           a,
           shared_memory_test::test_array_size);

    if(a[0]==shared_memory_test::concurrent_stop_value){
      break; // the loop stops here.
    }

    if(a[0]==shared_memory_test::concurrent_value_1){
      set_1_observed=true;
      nb_1++;
    }

    if(a[0]==shared_memory_test::concurrent_value_2){
      set_2_observed=true;
      nb_2++;
    }
    
    ASSERT_EQ(is_one_of(a[0],
      shared_memory_test::concurrent_value_1,
      shared_memory_test::concurrent_value_2),
        true);

    for(int i=1;i<shared_memory_test::test_array_size;i++){
      ASSERT_EQ(a[0],a[i]);
    }

    usleep(50);
    
  }

  ASSERT_EQ(set_1_observed,true);
  ASSERT_EQ(set_2_observed,true);
  
}

TEST_F(Shared_memory_tests,test_synchronisation){

  usleep(TIME_SLEEP);

  // create a data vector
  double d[shared_memory_test::test_array_size];

  // get a condition variable
  shared_memory::ConditionVariable cond_var (shared_memory_test::segment_id,
                                             shared_memory_test::cond_var_id);

  _call_executable(shared_memory_test::sync);

  cond_var.lock_scope();
  cond_var.wait();

  shared_memory::get(shared_memory_test::segment_id,
           shared_memory_test::object_id,
           d,
           shared_memory_test::test_array_size);

  ASSERT_EQ(d[0], shared_memory_test::concurrent_value_2);
  for(int i=1;i<shared_memory_test::test_array_size;i++){
    ASSERT_EQ(d[0],d[i]);
  }

  cond_var.notify_all();
  cond_var.wait();

  shared_memory::get(shared_memory_test::segment_id,
           shared_memory_test::object_id,
           d,
           shared_memory_test::test_array_size);

  ASSERT_EQ(d[0], shared_memory_test::concurrent_stop_value);
  for(int i=1;i<shared_memory_test::test_array_size;i++){
    ASSERT_EQ(d[0],d[i]);
  }

  cond_var.notify_all();
  cond_var.unlock_scope();
}

TEST_F(Shared_memory_tests,test_timed_wait){

  // get a condition variable
  shared_memory::ConditionVariable cond_var (
        shared_memory_test::segment_id,
        shared_memory_test::cond_var_id);
  cond_var.lock_scope();
  ASSERT_FALSE(cond_var.timed_wait(10));
  cond_var.unlock_scope();
}

