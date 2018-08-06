#include <set>
#include <string>
#include <map>
#include <memory>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include <iostream>

#define SHARED_MEMORY_DEFAULT_SIZE 1024*64


// cool doc: https://theboostcpplibraries.com/boost.interprocess-managed-shared-memory


namespace shared_memory {


  void _add_segment(const std::string &segment_id);
  void _add_mutex(const std::string &object_id);
  void _add(const std::string &segment_id,
	    const std::string &object_id);
  void _clear_all_mutexes();
  void _clear_all_segments();
  
  void clear_mutexes(const std::vector<std::string> &mutexes);
  void clear();
  

  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   T &set_){

    _add(segment_id,object_id);

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,
	                                               segment_id.c_str(),
	                                               SHARED_MEMORY_DEFAULT_SIZE};
    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create,object_id.c_str()};

    mutex.lock();
    T* object = segment.find_or_construct<T>(object_id.c_str())();
    *object = set_;
    mutex.unlock();

  }


  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   T *set_,
	   std::size_t size){

    _add(segment_id,object_id);

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_DEFAULT_SIZE};
    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

    mutex.lock();
    T* object = segment.find_or_construct<T>(object_id.c_str())[size]();
    for(int i=0;i<size;i++) object[i]=set_[i];
    mutex.unlock();

  }

  
  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   const std::vector<T> &set_){

    _add(segment_id,object_id);

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_DEFAULT_SIZE};
    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

    mutex.lock();
    T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
    for(int i=0;i<set_.size();i++) object[i]=set_[i];
    mutex.unlock();
    
  }



  template<typename KEY, typename VALUE>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   const std::map<KEY,VALUE> &set_){

    std::string keys_object_id = std::string("key_")+object_id;
    std::string values_object_id = std::string("values_")+object_id;

    std::vector<KEY> keys;
    std::vector<VALUE> values;
    for(auto const& x: set_){
      keys.push_back(x.first);
      values.push_back(x.second);
    }

    set<KEY>(segment_id,keys_object_id,keys);
    set<VALUE>(segment_id,values_object_id,values);

  }

  
  template<typename T>
  bool get(const std::string &segment_id,
	   const std::string &object_id,
	   T &get_) {

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_DEFAULT_SIZE};
    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

    mutex.lock();
    get_ = * ( segment.find_or_construct<T>(object_id.c_str())() );
    mutex.unlock();

  }

  


  
  
  template<typename T>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   T *get_,
	   std::size_t expected_size){

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_DEFAULT_SIZE};
    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

    mutex.lock();
    std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
    if(object.second != expected_size){
      throw std::runtime_error("shared memory error");
    }
    for(int i=0;i<object.second;i++) get_[i]=(object.first)[i];
    mutex.unlock();

  }


  

  
  template<typename T>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   std::vector<T> &get_) {

    boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_DEFAULT_SIZE};
    boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
  
    mutex.lock();
    std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
  
    if(object.second != get_.size()){
      get_.resize(object.second);
    }
    for(int i=0;i<object.second;i++) get_[i]=(object.first)[i];
    mutex.unlock();

  }


  template<typename KEY, typename VALUE>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   std::map<KEY,VALUE> &get_){

    std::string keys_object_id = std::string("key_")+object_id;
    std::string values_object_id = std::string("values_")+object_id;

    std::vector<KEY> keys;
    std::vector<VALUE> values;
    
    get(segment_id,keys_object_id,keys);
    get(segment_id,values_object_id,values);

    for(int i=0;i<keys.size();i++){
      get_[keys[i]]=values[i];
    }

  }

}
