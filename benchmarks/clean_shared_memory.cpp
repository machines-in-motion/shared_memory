#include "shared_memory/benchmarks/benchmark_common.hh"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

int main(){
  boost::interprocess::named_mutex::remove(SHM_NAME.c_str());
  boost::interprocess::shared_memory_object::remove(SHM_OBJECT_NAME.c_str());
}
