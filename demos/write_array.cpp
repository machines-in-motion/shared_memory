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

  shared_memory::clear_array(SEGMENT);
  
  shared_memory::array<shared_memory::Item<SIZE>> a( SEGMENT,
						     SIZE,
						     true,
						     true );

  int count=0;
  
  while(RUNNING)
    {
      for(int i=0;i<SIZE;i++)
	{
	  shared_memory::Item<SIZE> item(0);
	  item.set(i,count);
	  item.compact_print();
	  a.set(i,item);
	}
      std::cout << std::endl;
      count++;
      if(count==10)
	{
	  count=0;
	}
      usleep(100000);
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
