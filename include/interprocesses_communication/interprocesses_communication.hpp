#include <set>
#include <string>
#include <map>
#include <memory>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#define IC_DEFAULT_SIZE 1024*64


// cool doc: https://theboostcpplibraries.com/boost.interprocess-managed-shared-memory

namespace interprocesses_communication {

  template<typename T>
  bool get(const std::string &segment_id,
	   const std::string &object_id,
	   T &get) {

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create, segment_id.c_str(), IC_DEFAULT_SIZE};

    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, "mtx"};
    mutex.lock();
    get = * ( segment.find_or_construct<T>(object_id.c_str())() );
    mutex.unlock();

  }

  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   T &set){

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create, segment_id.c_str(), IC_DEFAULT_SIZE};
    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, "mtx"};
    mutex.lock();
    T* object = segment.find_or_construct<T>(object_id.c_str())();
    *object = set;
    mutex.unlock();

  }
  

}
