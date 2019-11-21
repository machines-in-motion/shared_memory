#include "shared_memory/array.hpp"
#include "shared_memory/demos/pitoune.hpp"
#include <signal.h>
#include <unistd.h>

#define SIZE 100
#define SEGMENT "demo_array"


static bool RUNNING = true;

void stop(int){
  RUNNING=false;
}


void run()
{

  shared_memory::array<Pitoune>::clear(SEGMENT);
  
  shared_memory::array<Pitoune> a(SEGMENT,
				  SIZE,
				  true,
				  true);

  Pitoune p(100);
  p.print();
  a.set(5,p);
  
  shared_memory::array<Pitoune> b(SEGMENT,
				  SIZE,
				  false,
				  true);

  uint iteration = 0;
  
  while(RUNNING)
    {
      usleep(100);
      /*
      for(int i=0;i<SIZE;i++)
	{
	  Pitoune p(iteration);
	  p.print();
	  a.set(i,p);
	}
      Pitoune p = b.get(5);
      p.print();
      usleep(10);
      iteration++;*/
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
