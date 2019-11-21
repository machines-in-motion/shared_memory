#include "shared_memory/array.hpp"
#include "shared_memory/demos/item.hpp"
#include <signal.h>
#include <unistd.h>

#define S 10
#define SIZE 100
#define SEGMENT "demo_array"


static bool RUNNING = true;

void stop(int){
  RUNNING=false;
}


void run()
{

  shared_memory::array<shared_memory::Item<S>> b(SEGMENT,
						 SIZE,
						 false,
						 true);

  while(RUNNING)
    {
      shared_memory::Item<S> n = b.get(5);
      n.print();
      usleep(12);
    }
  
}


int main()
{
  
  // stop on ctrl+c 
  struct sigaction cleaning;
  cleaning.sa_handler = stop;
  sigemptyset(&cleaning.sa_mask);
  cleaning.sa_flags = 0;
  sigaction(SIGINT, &cleaning, nullptr);

  run();

}
