#include "shared_memory/shared_memory.hpp"
// #include "shared_memory/elementary_messages.pb.h"
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
  m3["value_1"]=std::vector<double>(2, 0.0);
  m3["value_2"]=std::vector<double>(2, 0.0);

  std::map<std::string, Eigen::VectorXd> m4;
  m4["value_1"]=Eigen::VectorXd(4);
  m4["value_2"]=Eigen::VectorXd(4);

  std::string s1;

  // shared_memory::Arguments arguments;
  // std::string arguments_serialized;

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
    shared_memory::get("main_memory","s1",s1);
    // shared_memory::get("main_memory","protobuf_string",arguments_serialized);

    // arguments.ParseFromString(arguments_serialized);

    std::cout << "values: "                                          ;
    std::cout << "d1=" << d1 << " ; "                                ;
    std::cout << "d2=" << d2 << " ; "                                ;
    std::cout << "v1[0]=" << v1[0] << " ; "                          ;
    std::cout << "v1[1]=" << v1[1] << " ; "                          ;
    std::cout << "v2[0]=" << v2[0] << " ; "                          ;
    std::cout << "v2[1]=" << v2[1] << " ; "                          ;
    std::cout << "v3=" << v3.transpose() << " ; "                    ;
    std::cout << "m1[0]=" << m1[0] << " ; "                          ;
    std::cout << "m1[1]=" << m1[1] << " ; "                          ;
    std::cout << "m2[value1]=" << m2["value_1"] << " ; "             ;
    std::cout << "m2[value2]=" << m2["value_2"] << " ; "             ;
    std::cout << "m3[value1]=" << m3["value_1"][0] << " ; "          ;
    std::cout << "m3[value2]=" << m3["value_2"][0] << " ; "          ;
    std::cout << "m3[value1]=" << m3["value_1"][0] << " ; "          ;
    std::cout << "m3[value2]=" << m3["value_2"][0] << " ; "          ;
    std::cout << "m4[value1]=" << m4["value_1"].transpose() << " ; " ;
    std::cout << "m4[value2]=" << m4["value_2"].transpose() << " ; " ;
    std::cout << "s1=" << s1 << " ; " ;
    // std::cout << "nb args=" << arguments.args_size() << " ; " ;
    // std::cout << "args[" << 0 << "]=" << arguments.args(0).booleans(0) << " ; " ;
    // std::cout << "args[" << 1 << "]=" << arguments.args(1).integers(0) << " ; " ;
    std::cout << std::endl;
    usleep(10000);
  }
}
