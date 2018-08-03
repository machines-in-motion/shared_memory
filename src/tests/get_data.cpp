#include "interprocesses_communication/interprocesses_communication.hpp"
#include <iostream>

int main(){

  double d1;
  double d2;

  interprocess-communication::get<double>("main_memory","d1",d1);
  interprocess-communication::get<double>("main_memory","d2",d2);

  std::cout << "d1: " << d1 << std::endl;
  std::cout << "d2: " << d2 << std::endl;
  
}
