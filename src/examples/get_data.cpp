#include "shared_memory/shared_memory.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>


int main(){

  double d1;
  double d2;

  double v1[2];

  std::vector<double> v2;

  std::map<int,double> v3;
  
  while (true){
  
    shared_memory::get<double>("main_memory","d1",d1);
    shared_memory::get<double>("main_memory","d2",d2);
    shared_memory::get<double>("main_memory","v1",v1,2);
    shared_memory::get<double>("main_memory","v2",v2);
    shared_memory::get<int,double>("main_memory","v3",v3);

    std::cout << "values: " << d1 << "\t" << d2 << "\t";
    std::cout << v1[0] << "\t" << v1[1] << "\t";
    std::cout << v2[0] << "\t" << v2[1] << "\t";
    std::cout << v3[0] << "\t" << v3[1] << "\n";
    
    usleep(10000);
    
  }
    
}
