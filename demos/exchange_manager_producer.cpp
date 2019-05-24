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
  shared_memory::Exchange_manager_producer<shared_memory::Four_int_values,QUEUE_SIZE> exchange ( SEGMENT_ID,
												 OBJECT_ID);



  // will be used to track ids that has been consumed
  // by the consummer
  std::deque<int> consumed_ids;

  int c = 1;
  
  while(RUNNING){

    // pushing to memory a random number of instances of Four_int_values

    int nb_items = _get_int(5);

    for(int item=0;item<nb_items;item++){

      shared_memory::Four_int_values fiv(c,c,c,c);

      // serializing fiv and writing it to shared memory
      exchange.set(fiv);

      std::cout << "produced: " << fiv.get_id() << " | " << c << "\n";

      c++;
      
    }

    // reading from shared_memory which
    // items have been consumed

    exchange.get(consumed_ids);

    // printing consumed item ids to console

    std::cout << "\n";
    while (!consumed_ids.empty()){
      int id = consumed_ids.front();
      consumed_ids.pop_front();
      std::cout << "\t\tconsumed: " << id << "\n";
    }
    std::cout << "\n";

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
