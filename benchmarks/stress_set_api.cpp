#include "shared_memory/benchmarks/benchmark_common.hh"
#include "shared_memory/shared_memory.hpp"


void cleaning_memory(int){
  RUNNING=false;
  //shared_memory::delete_segment(SHM_NAME);
  shared_memory::SharedMemorySegment & segment =
      shared_memory::get_segment(SHM_NAME);
  segment.destroy_mutex();
}

int main(){

  // cleaning on ctrl+c
  struct sigaction cleaning;
  cleaning.sa_handler = cleaning_memory;
  sigemptyset(&cleaning.sa_mask);
  cleaning.sa_flags = 0;
  sigaction(SIGINT, &cleaning, nullptr);

  int count = 0;
  RUNNING = true;
  MeasureTime meas_time;
  while(RUNNING){

    shared_memory::set(SHM_NAME, SHM_OBJECT_NAME, DATA);

    ++count;
    if(count == SIZE){
      meas_time.update();
      std::cout << meas_time << std::endl;
      count = 0;
    }
  }

  cleaning_memory(0);
}
