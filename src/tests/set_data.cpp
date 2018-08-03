#include "interprocesses_communication/interprocesses_communication.hpp"
#include <unistd.h>

int main(){

  double d1=5;
  double d2=6;
  
  interprocesses_communication::set<double>("main_memory",
				    "d1",
				    d1);

  interprocesses_communication::set<double>("main_memory",
				    "d2",
				    d2);
  
  while (true){

    usleep(1000000);

  }

}
