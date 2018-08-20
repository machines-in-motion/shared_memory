#include "shared_memory/shared_memory.hpp"
#include "shared_memory/tests/tests.h"
#include <iostream>

int main(int argc, char *argv[]){

  int command = atoi(argv[1]);

  std::string segment = shared_memory_test::segment_id;
  std::string object = shared_memory_test::object_id;
  
  if (command==shared_memory_test::set_double){
    shared_memory::set(segment,object,
		       shared_memory_test::test_double);
  }

  if(command==shared_memory_test::set_int){
    shared_memory::set(segment,object,
		       shared_memory_test::test_int);
  }

  if(command==shared_memory_test::set_float){
    shared_memory::set(segment,object,
		       shared_memory_test::test_float);
  }

  if(command==shared_memory_test::set_string){
    shared_memory::set(segment,object,
		       shared_memory_test::test_string);
  }

  if(command==shared_memory_test::set_vector){
    std::vector<double> v;
    for(int i=0;i<shared_memory_test::test_array_size;i++){
      v.push_back(shared_memory_test::test_array[i]);
    }
    shared_memory::set(segment,object,
		       v);
  }

  if(command==shared_memory_test::set_int_double_map){
    std::map<int,double> m;
    m[shared_memory_test::map_int_keys1]=shared_memory_test::map_value_1;
    m[shared_memory_test::map_int_keys2]=shared_memory_test::map_value_2;
    shared_memory::set(segment,object,
		       m);
  }

  if(command==shared_memory_test::set_string_double_map){
    std::map<std::string,double> m;
    m[shared_memory_test::map_string_keys1]=shared_memory_test::map_value_1;
    m[shared_memory_test::map_string_keys2]=shared_memory_test::map_value_2;
    shared_memory::set(segment,object,
		       m);
  }

  if(command==shared_memory_test::set_double_array){
    shared_memory::set(segment,object,
		       shared_memory_test::test_array,
		       shared_memory_test::test_array_size);
  }
    
  if(command==shared_memory_test::concurrent_1){
    double d[shared_memory_test::test_array_size];
    for(int i=0;i<shared_memory_test::test_array_size;i++){
      d[i]=shared_memory_test::concurrent_value_1;
    }
    for(int i=0;i<5000;i++){
      shared_memory::set(segment,object,
			 d,shared_memory_test::test_array_size);
      usleep(50);
    }
    for(int i=0;i<shared_memory_test::test_array_size;i++){
      d[i]=shared_memory_test::concurrent_stop_value;
    }
    shared_memory::set(segment,object,
		       d,shared_memory_test::test_array_size);
  }

  if(command==shared_memory_test::concurrent_2){
    double d[shared_memory_test::test_array_size];
    for(int i=0;i<shared_memory_test::test_array_size;i++){
      d[i]=shared_memory_test::concurrent_value_2;
    }
    for(int i=0;i<5000;i++){
      shared_memory::set(segment,object,
			 d,shared_memory_test::test_array_size);
      usleep(50);
    }
    for(int i=0;i<shared_memory_test::test_array_size;i++){
      d[i]=shared_memory_test::concurrent_stop_value;
    }
    shared_memory::set(segment,object,
		       d,shared_memory_test::test_array_size);
  }
    
}

