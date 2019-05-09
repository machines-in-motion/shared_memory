#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>


#define SEGMENT_ID "exchange_demo_segment"
#define OBJECT_ID "exchange_demo_object"


int main(){

  shared_memory::clear_shared_memory(SEGMENT_ID);
  shared_memory::delete_segment(SEGMENT_ID);

}
