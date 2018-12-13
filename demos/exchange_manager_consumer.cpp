#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>


#define SEGMENT_ID "exchange_demo_segment"
#define OBJECT_ID "exchange_demo_object"
#define EXCHANGE_SIZE 200

static bool RUNNING = true;

void stop(int){
  RUNNING=false;
}

static int _get_int(int max){
  return rand()%max;
}


void execute(){

  srand(time(NULL));

  // Four_int_values is a subclass of shared_memory/serializable,
  // i.e an object which can be serialized as an array of double
  shared_memory::Exchange_manager_consumer<shared_memory::Four_int_values> exchange ( SEGMENT_ID,
										      OBJECT_ID,
										      EXCHANGE_SIZE );


  // for error detection
  int previous_id = -1;
  int id;
  
  while(RUNNING){

    // values serialized in shared memory will be deserialized in this object
    shared_memory::Four_int_values fiv;

    // we arbitrary consider we can only consume 3 items per iteration
    for(int i=0;i<3;i++){

      // ready_to_consume is used to make sure producer and
      // consumer are in sync
      if (exchange.ready_to_consume() && !exchange.empty()){
	exchange.consume(fiv);
	fiv.print();
	id = fiv.get_id();
	previous_id=id;
      }

    }

    // informing producer which items have been consumed
    exchange.update_memory();

    // note : faster than producer,
    //        as otherwise the buffer of the producer
    //        would end up overflowing
    usleep(1000);

  }
  
}


int main(){

  RUNNING = true;

  // cleaning and exit on ctrl+c 
  struct sigaction cleaning;
  cleaning.sa_handler = stop;
  sigemptyset(&cleaning.sa_mask);
  cleaning.sa_flags = 0;
  sigaction(SIGINT, &cleaning, nullptr);

  execute();

}
