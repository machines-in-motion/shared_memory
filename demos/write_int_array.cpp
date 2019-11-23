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

  shared_memory::array<int>::clear(SEGMENT);
  
  shared_memory::array<int> a( SEGMENT,
			       SIZE,
			       true,
			       true );

  shared_memory::array<int> b( SEGMENT,
			       SIZE,
			       false,
			       true );

  uint iteration = 0;

  while(RUNNING)
    {
      for(int i=0;i<SIZE;i++)
	{
	  a.set(i,iteration);
	}
      int p = b.get(5);
      std::cout << p << std::endl;
      usleep(10);
      iteration++;
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
