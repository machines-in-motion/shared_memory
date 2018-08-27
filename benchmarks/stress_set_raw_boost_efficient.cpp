#include "shared_memory/benchmarks/benchmark_common.hh"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

void cleaning_memory(int){
  RUNNING=false;
  boost::interprocess::named_mutex::remove(SHM_NAME.c_str());
}


int main(){

  boost::interprocess::named_mutex::remove(SHM_NAME.c_str());

  // exiting on ctrl+c
  struct sigaction exiting;
  exiting.sa_handler = cleaning_memory;
  sigemptyset(&exiting.sa_mask);
  exiting.sa_flags = 0;
  sigaction(SIGINT, &exiting, nullptr);

  boost::interprocess::managed_shared_memory segment{
    boost::interprocess::open_or_create,
    SHM_NAME.c_str(),
    SHARED_MEMORY_SIZE
  };

  boost::interprocess::named_mutex mutex{
    boost::interprocess::open_or_create,
    SHM_OBJECT_NAME.c_str()
  };

  double* object = segment.find_or_construct<double>(
                     SHM_OBJECT_NAME.c_str())[SIZE]();

  int count = 0;
  RUNNING=true;
  MeasureTime meas_time;
  while(RUNNING){

    mutex.lock();
    for(int i=0;i<SIZE;i++)
    {
      object[i]=2.0;
    }
    mutex.unlock();

    ++count;
    if(count == SIZE){
      meas_time.update();
      std::cout << meas_time << std::endl;
      count = 0;
    }
  }
}
