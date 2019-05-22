/**
 * @file stress_get_api.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Benchmark on the get method of the API.
 */

#include <shared_memory/benchmarks/benchmark_common.hh>
#include "shared_memory/shared_memory.hpp"


void cleaning_memory(int){
  RUNNING=false;
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

  shared_memory::clear_shared_memory(SHM_NAME);
  shared_memory::get_segment_mutex(SHM_NAME).unlock();
  shared_memory::set(SHM_NAME, SHM_OBJECT_NAME, DATA);

  int count = 0;
  RUNNING=true;
  MeasureTime meas_time;
  while(RUNNING && count<MAX_NUNMBER_OF_ITERATION){

    shared_memory::get(SHM_NAME, SHM_OBJECT_NAME, DATA);

    ++count;
    if(count % NUMBER_OR_MEASURED_ITERATIONS == 0){
      meas_time.update();
      std::cout << meas_time << " | " << DATA[0] <<  std::endl;
    }
  }
}
