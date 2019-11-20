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

  shared_memory::array<Pitoune> b(SEGMENT,
				  SIZE,
				  false,
				  true);
  while(RUNNING)
    {
      std::cout << ".";
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
