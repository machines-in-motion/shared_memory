#include "shared_memory/array.hpp"
#include "shared_memory/demos/item.hpp"
#include <signal.h>
#include <unistd.h>


#define SIZE 4
#define SEGMENT "demo_array"


static bool RUNNING = true;

void stop(int){
  RUNNING=false;
}


void run()
{

  shared_memory::array<shared_memory::Item<SIZE>> a(SEGMENT,
						    SIZE,
						    false,
						    true);

  while(RUNNING)
    {
      shared_memory::Item<SIZE> item;
      
      for(int i=0;i<SIZE;i++)
	{
	  a.get(i,item);
	  item.compact_print();
	}

      std::cout << std::endl;

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
