#include "shared_memory/shared_memory.hpp"
#include <unistd.h>
#include <vector>
#include <iostream>
#include <signal.h>


static bool RUNNING = true;



// the shared memory does not keep track of what
// segment and objects it created (to be realtime compatible),
// so cleaning has to be "manual"

void cleaning_memory(int){

  RUNNING=false;

  std::cout << "\n\n--cleaning shared memory --\n\n";

  // objects that have been set to the shared memory
  std::vector<std::string> objects;
  objects.push_back("d1");
  objects.push_back("d2");
  objects.push_back("v1");
  objects.push_back("v2");
  objects.push_back("v3");
  objects.push_back("m1");

  // maps (and their relative keys) that have been set to
  // the shared memory
  std::map<std::string,std::vector<std::string>> map_keys;
  std::vector<std::string> keys;
  keys.push_back("value_1");
  keys.push_back("value_2");
  map_keys[std::string("m2")]=keys;
  map_keys[std::string("m3")]=keys;
  map_keys[std::string("m4")]=keys;

  shared_memory::clear("main_memory",objects,map_keys);

}


int main(){

  cleaning_memory(0);
  RUNNING = true;

  // cleaning on ctrl+c 
  struct sigaction cleaning;
  cleaning.sa_handler = cleaning_memory;
  sigemptyset(&cleaning.sa_mask);
  cleaning.sa_flags = 0;
  sigaction(SIGINT, &cleaning, nullptr);

  double d1=0.0;
  double d2=0.0;

  double v1[2];
  
  std::vector<double> v2(2, 0.0);

  Eigen::VectorXd v3 = Eigen::VectorXd::Random(4);

  std::map<int,double> m1;
  m1[0]=d1;
  m1[1]=d2;

  std::map<std::string,double> m2;
  m2["value_1"]=d1;
  m2["value_2"]=d2;

  std::map<std::string,std::vector<double>> m3;
  m3["value_1"]=std::vector<double>(1, 0.0);
  m3["value_2"]=std::vector<double>(1, 0.0);

  std::map<std::string, Eigen::VectorXd> m4;
  m4["value_1"]=Eigen::VectorXd::Random(4);
  m4["value_2"]=Eigen::VectorXd::Random(4);
  
  unsigned count = 0;
  while (RUNNING){

    d1+=0.01;
    d2+=0.001;

    v1[0]=(d1+1);
    v1[1]=(d2+1);

    v2[0]=(d1+2);
    v2[1]=(d2+2);

    v3+=Eigen::VectorXd::Ones(4)*0.001;

    m1[0]=(d1+3);
    m1[1]=(d2+3);

    m2["value_1"]=(d1+4);
    m2["value_2"]=(d2+4);

    m3["value_1"][0]=(d1+5);
    m3["value_2"][0]=(d2+5);

    m4["value_1"]+=Eigen::VectorXd::Ones(4)*0.05;
    m4["value_2"]+=Eigen::VectorXd::Ones(4)*0.02;
    
    shared_memory::set("main_memory","d1",d1);
    shared_memory::set("main_memory","d2",d2);
    shared_memory::set("main_memory","v1",v1,2);
    shared_memory::set("main_memory","v2",v2);
    shared_memory::set("main_memory","v3",v3);
    shared_memory::set("main_memory","m1",m1);
    shared_memory::set("main_memory","m2",m2);
    shared_memory::set("main_memory","m3",m3);
    shared_memory::set("main_memory","m4",m4);

    ++count;
    std::cout << ".";
    if(count % 100 == 0)
    {
      std::cout << std::endl;
    }
    usleep(1000);

  }
  
}
