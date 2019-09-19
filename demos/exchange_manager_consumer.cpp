/**
 * @file exchange_manager_consumer.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief Demonstrate how to use the exchange manage consummer
 */
#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>


#define SEGMENT_ID "exchange_demo_segment"
#define OBJECT_ID "exchange_demo_object"
#define QUEUE_SIZE 2000*4

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
  bool leading = false; // producer expected to start first, and to survive serveral consumers
  bool autolock = false; // to read several items in a single shot
  shared_memory::Exchange_manager_consumer<shared_memory::Four_int_values,
					   QUEUE_SIZE> exchange ( SEGMENT_ID,
								  OBJECT_ID,
								  leading,
								  autolock );


  while(RUNNING){

    // values serialized in shared memory will be deserialized in this object
    shared_memory::Four_int_values fiv;

    if(exchange.ready_to_consume()) {
    
      // required because autolock is false
      exchange.lock();
    
      // we arbitrary consider we can only consume 3 items per iteration
      for(int i=0;i<3;i++){

	bool consuming = exchange.consume(fiv);

	if(!consuming) break;

	fiv.print();
      
      }

      exchange.unlock();

    } else {

      std::cout << "waiting for producer\n";
      
    }
    
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
