#include "shared_memory/shared_memory.hpp"
#include <unistd.h>
#include <vector>
#include <iostream>

int main(){

  double d1=0.0;
  double d2=0.0;

  double v1[2];
  
  std::vector<double> v2;
  v2.push_back(d1);
  v2.push_back(d2);

  std::map<int,double> v3;
  v3[0]=d1;
  v3[1]=d2;

  
  while (true){

    d1+=0.01;
    d2+=0.001;

    v1[0]=(d1+1);
    v1[1]=(d2+1);

    v2[0]=(d1+2);
    v2[1]=(d2+2);

    v3[0]=(d1+3);
    v3[1]=(d2+3);
    
    shared_memory::set<double>("main_memory","d1",d1);
    shared_memory::set<double>("main_memory","d2",d2);
    shared_memory::set<double>("main_memory","v1",v1,2);
    shared_memory::set<double>("main_memory","v2",v2);
    shared_memory::set<int,double>("main_memory","v3",v3);
    
    std::cout << ".\n";
    
    usleep(1000);

  }

  
}
