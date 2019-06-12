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

  {
    shared_memory::ConditionVariable cv_ ("main_memory", "cond_var");
  }
    
  shared_memory::clear_shared_memory("main_memory");
  shared_memory::delete_segment("main_memory");

  {
    shared_memory::ConditionVariable cv_ ("main_memory", "cond_var");
  }
  
  shared_memory::ConditionVariable cv ("main_memory", "cond_var");

  int count = 0;
  shared_memory::set<int>("main_memory","count",count);

  bool started = false;

  cv.lock_scope();
  
  while (RUNNING) {

    if (!started){
      std::cout << "waiting for pong\n";
      started = true;
    }
  

    cv.wait();

    shared_memory::get<int>("main_memory","count",count);
    count++;
    shared_memory::set<int>("main_memory","count",count);
    
    std::cout << "\t\tping " << count << "\n";

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
