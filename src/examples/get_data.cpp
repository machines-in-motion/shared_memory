#include "shared_memory/shared_memory.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <signal.h>

static bool RUNNING = true;


void exiting_memory(int){

  RUNNING=false;

}


int main(){

  // exiting on ctrl+c 
  struct sigaction exiting;
  exiting.sa_handler = exiting_memory;
  sigemptyset(&exiting.sa_mask);
  exiting.sa_flags = 0;
  sigaction(SIGINT, &exiting, NULL);

  double d1=0.0;
  double d2=0.0;

  double v1[2];
  v1[0]=0.0;
  v1[1]=0.0;

  std::vector<double> v2(2);
  v2[0]=0.0;
  v2[1]=0.0;

  Eigen::VectorXd v3(4);
  
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
  m4["value_1"]=Eigen::VectorXd(4);
  m4["value_2"]=Eigen::VectorXd(4);

  
  while (RUNNING){

    shared_memory::get("main_memory","d1",d1);
    shared_memory::get("main_memory","d2",d2);
    shared_memory::get("main_memory","v1",v1,2);
    shared_memory::get("main_memory","v2",v2);
    shared_memory::get("main_memory","v3",v3);
    shared_memory::get("main_memory","m1",m1);
    shared_memory::get("main_memory","m2",m2);
    shared_memory::get("main_memory","m3",m3);
    shared_memory::get("main_memory","m4",m4);

    std::cout << "values: " << d1 << "\t" << d2 << "\t";
    std::cout << v1[0] << "\t" << v1[1] << "\t";
    std::cout << v2[0] << "\t" << v2[1] << "\t";
    std::cout << v3.transpose() << "\t";
    std::cout << m1[0] << "\t" << m1[1] << "\t";
    std::cout << m2["value_1"] << "\t" << m2["value_2"] << "\t";
    std::cout << m3["value_1"][0] << "\t" << m3["value_2"][0] << "\n";
    std::cout << m4["value_1"].transpose() << m4["value_2"].transpose() << "\n";
    
    usleep(10000);
    
  }
    
}
