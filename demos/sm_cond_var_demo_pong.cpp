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
#define VALUE 1


void update_vector(){

  
  std::vector<int> v(1000000);
  shared_memory::Mutex::clean(MUTEX_ID);
  shared_memory::Mutex mutex(MUTEX_ID);
  shared_memory::ConditionVariable condition(CV_SEGMENT_ID,true);

  // starting ping
  std::cout << "\nstarting PING...\n";
  condition.notify_one();
  std::cout << "\nstarting PONG...\n";
  
  for(int i=0;i<10000;i++){

    {

      shared_memory::Lock lock(mutex);
      condition.wait(lock);

      std::cout << "PONG ";
      for(int i=0;i<v.size();i++){
	v[i] = VALUE;
      }
    
      usleep(500);
    
      for(int i=0;i<v.size();i++){
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

  // stopping ping
  shared_memory::set<bool>(RUNNING_SEGMENT_ID,
			   RUNNING_OBJECT_ID,false);

  
}

int main(){

  bool go_ahead;
  
  try {
    // ping should be started first.
    // if it is starting, below will not fail
    bool foo;
    shared_memory::get<bool>(RUNNING_SEGMENT_ID,
			     RUNNING_OBJECT_ID,foo);
    go_ahead=true;
  } catch(...){
    std::cout << "\nstart sm_cond_var_ping first !\n\n";
  }

  if(go_ahead){
    shared_memory::set<bool>(RUNNING_SEGMENT_ID,
			     RUNNING_OBJECT_ID,true);
    update_vector();
  }
  
}
