#include "interprocess-communication/interprocess-communication.hpp"
#include <unistd.h>

int main(){

  double first_double=5;
  double second_double=6;
  
  interprocess_communication::admin::Segment<double>::create("main_memory",
							     "first_double",
							     first_double);

  interprocess_communication::admin::Segment<double>::create("main_memory",
							     "second_double",
							     second_double);
  
  while (true){

    usleep(1000000);

  }

}
