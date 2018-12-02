#include "shared_memory/exchange_manager_producer.hpp"
#include "shared_memory/demos/four_int_values.hpp"

#define SEGMENT_ID "exchange_demo_segment"
#define OBJECT_ID "exchange_demo_object"
#define EXCHANGE_SIZE 200

  
int main(){

  shared_memory::Exchange_manager_producer<shared_memory::Four_int_values> exchange ( SEGMENT_ID,
										      OBJECT_ID,
										      EXCHANGE_SIZE );

  exchange.clean_memory();

}
