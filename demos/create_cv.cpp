#include <boost/interprocess/sync/named_condition.hpp>
#include "shared_memory/condition_variable.hpp"
#include <iostream>
#include <unistd.h>

#define CV_OBJECT_ID "pouet"



int main() {

  std::string condition_id(CV_OBJECT_ID);
  shared_memory::ConditionVariable cv0(CV_OBJECT_ID,false);
  boost::interprocess::named_condition condition_variable_(
							   boost::interprocess::open_or_create,
							   condition_id.c_str());

  shared_memory::ConditionVariable cv1(CV_OBJECT_ID,false);
  shared_memory::ConditionVariable cv2(CV_OBJECT_ID,false);
  
  std::cout << "\n\nok\n\n";
  
}
