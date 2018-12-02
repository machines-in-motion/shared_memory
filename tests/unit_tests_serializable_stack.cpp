#include "shared_memory/serializable_stack.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include "shared_memory/tests/tests.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <memory>

#define BUFFER_SIZE 4

using namespace shared_memory;


class Serializable_stack_tests : public ::testing::Test {
protected:
  void SetUp() {}
  void TearDown() {}
};


static void copy_data(double *src, double *dest, int size){
  std::memcpy(dest,src,size*sizeof(double));
}

static void print_data(double *d, int size){
  std::cout << "\n\nDATA: " << size <<"\n";
  for(int i=0;i<size;i++){
    std::cout << d[i] << " " ;
  }
  std::cout << "\n\n";
}


TEST_F(Serializable_stack_tests,size_array){
  Serializable_stack<Four_int_values> stack(BUFFER_SIZE);
  Serializable_stack_reader<Four_int_values> stack_reader(BUFFER_SIZE);
  ASSERT_EQ(stack.get_data_size(),stack_reader.get_data_size());
}


TEST_F(Serializable_stack_tests,write_and_read){
  Serializable_stack<Four_int_values> stack(BUFFER_SIZE);
  Serializable_stack_reader<Four_int_values> stack_reader(BUFFER_SIZE);
  Four_int_values p1(1,2,3,4);
  Four_int_values p2(5,6,7,8);
  stack.add(p1);
  stack.add(p2);
  double *data_stack = stack.get_data();
  double *data_reader = stack_reader.get_data_and_reset();
  copy_data(data_stack,data_reader,stack.get_data_size());
  ASSERT_EQ(stack_reader.empty(),false);
  Four_int_values e1,e2;
  stack_reader.read(e1);
  ASSERT_EQ(stack_reader.empty(),false);
  stack_reader.read(e2);
  ASSERT_EQ(stack_reader.empty(),true);
  ASSERT_EQ(p1.same(e1),true);
  ASSERT_EQ(p2.same(e2),true);
}


TEST_F(Serializable_stack_tests,exception_thrown){
  Serializable_stack<Four_int_values> stack(BUFFER_SIZE);
  Four_int_values p1(1,2,3,4);
  Four_int_values p2(5,6,7,8);
  Four_int_values p3(1,2,3,4);
  Four_int_values p4(5,6,7,8);
  Four_int_values p5(5,6,7,8);
  stack.add(p1);
  stack.add(p2);
  stack.add(p3);
  stack.add(p4);
  ASSERT_THROW ( stack.add(p5) , std::overflow_error );
}


TEST_F(Serializable_stack_tests,items_removals){
  Serializable_stack<Four_int_values> stack(BUFFER_SIZE);
  Serializable_stack_reader<Four_int_values> stack_reader(BUFFER_SIZE);
  Four_int_values p1(1,2,3,4);
  Four_int_values p2(5,6,7,8);
  stack.add(p1);
  stack.add(p2);
  double *data_stack = stack.get_data();
  double *data_reader = stack_reader.get_data_and_reset();
  copy_data(data_stack,data_reader,stack.get_data_size());
  Four_int_values e1,e2;
  stack_reader.read(e1);
  stack_reader.read(e2);
  std::deque<int> removed_ids;
  stack.remove(2,removed_ids);
  ASSERT_EQ(removed_ids.size(),2);
  ASSERT_EQ(removed_ids.back(),e2.get_id());
  removed_ids.pop_back();
  ASSERT_EQ(removed_ids.back(),e1.get_id());
  removed_ids.pop_back();
  ASSERT_EQ(removed_ids.empty(),true);
  Four_int_values p3(9,10,11,12);
  stack.add(p3);
  data_reader = stack_reader.get_data_and_reset();
  copy_data(data_stack,data_reader, stack.get_data_size());
  Four_int_values e3;
  stack_reader.read(e3);
  ASSERT_EQ(p3.same(e3),true);
}

