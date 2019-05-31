#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>


#define SEGMENT_ID "exchange_demo_segment"
#define QUEUE_SIZE 2000


int main(){

  shared_memory::Mutex locker_(std::string(SEGMENT_ID)+"_locker",true);
  shared_memory::Exchange_manager_consumer<shared_memory::Four_int_values,QUEUE_SIZE>::clean_memory(SEGMENT_ID);

}
