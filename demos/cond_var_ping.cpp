#include <vector>
#include <atomic>
#include <unistd.h>
#include <thread>
#include <iostream>

#include "shared_memory/condition_variable.hpp"
#include "shared_memory/mutex.hpp"
#include "shared_memory/lock.hpp"

#define MUTEX_ID "sm_cond_var_demo_mutex"
#define CV_SEGMENT_ID "sm_cond_var_demo_segment"
#define RUNNING_SEGMENT_ID "sm_running_demo_segment"
#define RUNNING_OBJECT_ID "sm_running_demo_object"
#define VALUE 2


bool should_run(){
  bool running;
  shared_memory::get<bool>(RUNNING_SEGMENT_ID,
			   RUNNING_OBJECT_ID,running);
  return running;
}

void update_vector(){

  
  std::vector<int> v(1000000);
  shared_memory::Mutex::clean(MUTEX_ID);
  shared_memory::clear_shared_memory(RUNNING_SEGMENT_ID);
  shared_memory::Mutex mutex(MUTEX_ID);
  shared_memory::ConditionVariable condition(CV_SEGMENT_ID,true);
  shared_memory::set<bool>(RUNNING_SEGMENT_ID,
			   RUNNING_OBJECT_ID,false);

  std::cout << "\nwaiting for pong ...\n";

  bool first_iteration = true;
  
  while(first_iteration || should_run()){

    first_iteration=false;

    {

      shared_memory::Lock lock(mutex);
      condition.wait(lock);

      std::cout << "PING ";
      for(unsigned int i=0;i<v.size();i++){
	v[i] = VALUE;
      }
    
      usleep(500);
    
      for(unsigned int i=0;i<v.size();i++){
	if (v[i]!= VALUE){
	  std::cout << "\n--- damn, the vector was not locked !\n";
	  break;
	}
	if (i<10){
	  std::cout << " " << v[i];
	}
      }
      std::cout << "\n";

    }

    condition.notify_one();
    
  }

  shared_memory::clear_shared_memory(RUNNING_SEGMENT_ID);
  
}

int main(){

  update_vector();
  
}
