#include "shared_memory/thread_synchronisation.hpp"
#include <iostream>
#include <thread>


static int thread_callback()
{
  std::cout << "THREAD: create the condition variable" << std::endl;
  // get a condition variable
  shared_memory::ConditionVariable cond_var ("main_memory", "cond_var");

  cond_var.lock_scope();
  std::cout << "THREAD: wait" << std::endl;
  cond_var.wait();
  std::cout << "THREAD: sleep(11)" << std::endl;
  sleep(11);
  std::cout << "THREAD: notify all" << std::endl;
  cond_var.notify_all();
  return 0;
}


int main(){
  shared_memory::delete_segment("main_memory");
  sleep(1);
  shared_memory::get_segment("main_memory");
  sleep(1);
  std::cout << "MAIN: create thread" << std::endl;
  std::thread my_thread (&thread_callback);
  std::cout << "MAIN: sleep(1) so THREAD goes to wait" << std::endl;
  sleep(1);
  std::cout << "MAIN: create the condition variable" << std::endl;
  shared_memory::ConditionVariable cond_var ("main_memory", "cond_var");

  std::cout << "MAIN: notify thread" << std::endl;
  cond_var.notify_all();

  std::cout << "MAIN: timed wait" << std::endl;
  if(!cond_var.timed_wait(5))
  {
    std::cout<< "MAIN: TIMED_OUT!!!" << std::endl;
  }else{
    std::cout<< "MAIN: Has been notified" << std::endl;
  }
  cond_var.notify_all();
  my_thread.join();
}
