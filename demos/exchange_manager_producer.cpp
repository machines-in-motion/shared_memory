#include "shared_memory/exchange_manager_producer.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>


#define SEGMENT_ID "exchange_demo_segment"
#define OBJECT_ID "exchange_demo_object"
#define QUEUE_SIZE 2000

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
  bool autolock = false; // we need to "manually" call lock, in order to write several item in a single shot
  bool leading = true; // producer expected to start first, and to survive several consumers
  shared_memory::Exchange_manager_producer<shared_memory::Four_int_values,QUEUE_SIZE> exchange ( SEGMENT_ID,
												 OBJECT_ID,
												 leading,
												 autolock );




  // will be used to track ids that has been consumed
  // by the consummer
  std::deque<int> consumed_ids;

  int c = 1;

  // to be used to inform user producer is waiting because
  // memory is full
    
  bool waiting_warning_printed = false;

  while(RUNNING){

    
    // pushing to memory a random number of instances of Four_int_values

    int nb_items = _get_int(5);

    if ( exchange.ready_to_produce() ){
    
      // necessary because autolock is false
      exchange.lock();
    
      for(int item=0;item<nb_items;item++){

	shared_memory::Four_int_values fiv(c,c,c,c);

	// serializing fiv and writing it to shared memory
	try {
	
	  exchange.set(fiv);
	  waiting_warning_printed = false;
	  std::cout << "produced: " << fiv.get_id() << " | " << c << "\n";
	  c++;
	
	} catch (shared_memory::Memory_overflow_exception){

	  if (!waiting_warning_printed){
	    waiting_warning_printed = true;
	    std::cout << "shared memory full, waiting for consumer ...\n";
	  }
	} 
      
      }

      exchange.unlock();

    } else {

      std::cout << "waiting for consumer to start ...\n" ;
      
    }

    // reading from shared_memory which
    // items have been consumed

    exchange.lock();
    exchange.get(consumed_ids);
    exchange.unlock();
    
    // printing consumed item ids to console

    while (!consumed_ids.empty()){
      int id = consumed_ids.front();
      consumed_ids.pop_front();
      std::cout << "\t\tconsumed: " << id << "\n";
    }

    // note : slower than consumer,
    //        as otherwise the buffer
    //        would end up overflowing

    usleep(2000);

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
