#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <chrono>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>


#define _SHARED_MEMORY_SIZE 65536
#define SIZE 1000


static bool RUNNING; 

void exit_(int){
  RUNNING=false;
}


int main(){

  // exiting on ctrl+c 
  struct sigaction exiting;
  exiting.sa_handler = exit_;
  sigemptyset(&exiting.sa_mask);
  exiting.sa_flags = 0;
  sigaction(SIGINT, &exiting, NULL);
  
  
  std::string segment_id("stress_test");
  std::string object_id("stress_object");
    
  int count = 0;
  
  std::chrono::high_resolution_clock::time_point previous_t = std::chrono::high_resolution_clock::now();
  
  RUNNING=true;

  std::chrono::high_resolution_clock::time_point t;
  
  while(RUNNING){

    if (count == 0)
    {
      t = std::chrono::high_resolution_clock::now();
    }
    
    boost::interprocess::managed_shared_memory segment{
      boost::interprocess::open_or_create,
      segment_id.c_str(),
      _SHARED_MEMORY_SIZE
    };
  
    boost::interprocess::named_mutex mutex{
      boost::interprocess::open_or_create,
      object_id.c_str()
    };

    mutex.lock();
    double* object = segment.find_or_construct<double>(object_id.c_str())[SIZE]();
    for(int i=0;i<SIZE;i++) object[i]=2.0;
    mutex.unlock();

    count++;
    if(count == 1000){
      auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(t-previous_t).count();
      float frequency = static_cast<float>(1000) / ( pow(10.0,-9.0) * static_cast<float>(nanos) );
      previous_t = t;
      std::cout << "frequency: " << frequency << "\n";
      count = 0;
    }
  }

  boost::interprocess::named_mutex::remove(object_id.c_str());
  boost::interprocess::shared_memory_object::remove(segment_id.c_str());


}
