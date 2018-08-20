#include "shared_memory/shared_memory.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <signal.h>

static bool RUNNING = true;

// the shared memory does not keep track of what
// segment and objects it created (to be realtime compatible),
// so cleaning has to be "manual"

void cleaning_memory(int){

  RUNNING=false;

  std::cout << "\n\n--cleaning shared memory --\n\n";

  std::vector<std::string> keys;
  keys.push_back("d1");
  keys.push_back("d2");
  keys.push_back("v1");
  keys.push_back("v2");
  keys.push_back("m1");
  keys.push_back("m2");
  shared_memory::clear("main_memory",keys);

}


int main(){

  // cleaning on ctrl+c 
  struct sigaction cleaning;
  cleaning.sa_handler = cleaning_memory;
  sigemptyset(&cleaning.sa_mask);
  cleaning.sa_flags = 0;
  sigaction(SIGINT, &cleaning, NULL);

  double d1=0.0;
  double d2=0.0;

  double v1[2];
  v1[0]=0.0;
  v1[1]=0.0;

  std::vector<double> v2(2);
  v2[0]=0.0;
  v2[1]=0.0;

  
  std::map<int,double> m1;
  m1[0]=d1;
  m1[1]=d2;

  std::map<std::string,double> m2;
  m2["value_1"]=d1;
  m2["value_2"]=d2;
  
  while (true){
  
    shared_memory::get("main_memory","d1",d1);
    shared_memory::get("main_memory","d2",d2);
    shared_memory::get("main_memory","v1",v1,2);
    shared_memory::get("main_memory","v2",v2);
    shared_memory::get("main_memory","m1",m1);
    shared_memory::get("main_memory","m2",m2);

    std::cout << "values: " << d1 << "\t" << d2 << "\t";
    std::cout << v1[0] << "\t" << v1[1] << "\t";
    std::cout << v2[0] << "\t" << v2[1] << "\t";
    std::cout << m1[0] << "\t" << m1[1] << "\t";
    std::cout << m2["value_1"] << "\t" << m2["value_2"] << "\n";
    
    usleep(10000);
    
  }
    
}
