#include "shared_memory/thread_synchronisation.hpp"
#include "shared_memory/shared_memory.hpp"
#include <iostream>
#include <thread>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>


static bool RUNNING = true;

void stop(int){
  RUNNING=false;
}


void execute(){

  shared_memory::ConditionVariable cv ("main_memory", "cond_var",false);

  cv.lock_scope();
  
  // ping was waiting for pong to start,
  // informing ping it can go ahead
  cv.notify_one();
  
  while (RUNNING) {
  
    cv.wait();

    int count;
    shared_memory::get<int>("main_memory","count",count);
    count++;
    shared_memory::set<int>("main_memory","count",count);
    
    std::cout << "\t\tpong " << count << "\n";

    usleep(2000);

    cv.notify_one();
    
  }
  
  cv.unlock_scope();
  
}


int main(){

  // cleaning and exit on ctrl+c 
  struct sigaction cleaning;
  cleaning.sa_handler = stop;
  sigemptyset(&cleaning.sa_mask);
  cleaning.sa_flags = 0;
  sigaction(SIGINT, &cleaning, nullptr);

  execute();
  
}
