#include "shared_memory/exchange_manager_producer.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>


#define SEGMENT_ID "exchange_demo_segment"
#define QUEUE_SIZE 2000


int main(){

  shared_memory::Exchange_manager_producer<shared_memory::Four_int_values,
					   QUEUE_SIZE>::clean_mutex(std::string(SEGMENT_ID));
  shared_memory::Exchange_manager_producer<shared_memory::Four_int_values,
					   QUEUE_SIZE>::clean_memory(std::string(SEGMENT_ID));

}
