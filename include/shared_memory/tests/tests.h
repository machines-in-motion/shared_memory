#include <string>
#include <map>
#include <vector>



namespace shared_memory_test {

  const std::string segment_id("unit_test_segment");
  const std::string object_id("unit_test_object");
  
  const double test_double = 4.4;
  const float test_float = 4.4;
  const int test_int = 3;
  const std::string test_string("test_string");
  const double test_array[4]={1.1,2.2,3.3,4.4};
  const int test_array_size = 4;
  const int map_int_keys1 = 0;
  const int map_int_keys2 = 1;
  const std::string map_string_keys1("s1");
  const std::string map_string_keys2("s2");
  const double map_value_1 = 3.3;
  const double map_value_2 = 4.4;
  const double concurrent_value_1 = 1.0;
  const double concurrent_value_2 = 2.0;
  const double concurrent_stop_value = 3.0;
  
  const int test_map_size = 2;

  const int set_double = 1;
  const int set_int = 2;
  const int set_float = 3;
  const int set_string = 4;
  const int set_vector = 5;
  const int set_int_double_map = 6;
  const int set_string_double_map = 7;
  const int set_double_array = 8;
  const int set_string_vector_double_map = 9;
  const int concurrent_1 = 10;
  const int concurrent_2 = 11;
  
  
 
}
