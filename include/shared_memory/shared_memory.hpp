#include <set>
#include <string>
#include <map>
#include <memory>
#include <iostream>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "shared_memory/exceptions.h"



#define _SHARED_MEMORY_SIZE 65536
#define _MAP_STRING_KEY_SEPARATOR ';'



// cool doc: https://theboostcpplibraries.com/boost.interprocess-managed-shared-memory
//           https://www.boost.org/doc/libs/1_63_0/doc/html/interprocess/quick_guide.html



namespace shared_memory {

  void clear_segment(const std::string &segment_id);
  void clear_mutex(const std::string &object_id);
  void clear_mutexes(const std::vector<std::string> &mutexes);
  void clear(const std::string &segment_id,
	     const std::vector<std::string> &mutexes);

  void set(const std::string &segment_id,
	   const std::string &object_id,
	   const std::string &set_);

  
  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   const T &set_){

    // if T turns out to be a std::string,
    // calling specialized function
    if (std::is_same<T, std::string>::value){
      set(segment_id,object_id,set_);
      return;
    }
    
    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,
	  segment_id.c_str(),_SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create,object_id.c_str()};

      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())();
      *object = set_;
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

  }


  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   const T *set_,
	   std::size_t size){

    try {
    
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),_SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[size]();
      for(int i=0;i<size;i++) object[i]=set_[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

  }


  
  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   const std::vector<T> &set_){

    try {
    
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),_SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
      for(int i=0;i<set_.size();i++) object[i]=set_[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

    
  }


  template<typename T>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   std::vector<T*> &set_){

    try {
    
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),_SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
      for(int i=0;i<set_.size();i++) {
	object[i]=*(set_[i]);
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
      
  }


  
  template<typename KEY, typename VALUE>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   std::map<KEY,VALUE> &set_){

    try {
    
      std::string keys_object_id = std::string("key_")+object_id;
      std::string values_object_id = std::string("values_")+object_id;
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),_SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      KEY* keys = segment.find_or_construct<KEY>(keys_object_id.c_str())[set_.size()]();
      VALUE* values = segment.find_or_construct<VALUE>(values_object_id.c_str())[set_.size()]();
      int i=0;
      for (const auto& key_value: set_){
	keys[i]=key_value.first;
	values[i]=key_value.second;
	i++;
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

  }


  template<typename VALUE>
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   std::map<std::string,VALUE> &set_){


    try {
    
      std::string keys_object_id = std::string("key_")+object_id;
      std::string values_object_id = std::string("values_")+object_id;
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),_SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

      // keys are stored as one key string with ';' (MAP_STRING_KEY_SEPARATOR) separator
      std::stringstream key;
      
      mutex.lock();

      VALUE* values = segment.find_or_construct<VALUE>(values_object_id.c_str())[set_.size()]();
      int i=0;
      for (const auto& key_value: set_){
	key << key_value.first << _MAP_STRING_KEY_SEPARATOR;
	values[i]=key_value.second;
	i++;
      }
      mutex.unlock();

      std::string key_ = key.str();
      set<std::string>(segment_id,keys_object_id,key_);

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
    
  }

  
  


  
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   std::string &get_);


  template<typename T>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   T &get_) {


    // if T turns out to be std::string,
    // calling specialized function
    if (std::is_same<T, std::string>::value){
      get(segment_id,object_id,get_);
      return;
    }
    
    try {
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      
      mutex.lock();
      get_ = * ( segment.find_or_construct<T>(object_id.c_str())() );
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
      
    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }
      
  }
  
  
  template<typename T>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   T *get_,
	   std::size_t expected_size){

    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
      if(object.second != expected_size){
	throw shared_memory::Unexpected_size_exception(segment_id,
						       object_id,
						       object.second,
						       expected_size);
      }
      for(int i=0;i<object.second;i++) get_[i]=(object.first)[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }

      
  }


  template<typename T>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   std::vector<T> &get_) {

    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
      if(object.second != get_.size()){
	throw shared_memory::Unexpected_size_exception(segment_id,
						       object_id,
						       object.second,
						       get_.size());
      }
      
      for(int i=0;i<object.second;i++) get_[i]=(object.first)[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }

      
  }



  // convenience function for splitting an std::string into
  // strings based on separator.
  static void _get_next(const char *a, char separator, int start, int &end){
    end = start;
    while (a[end]!=separator){
      end+=1;
    }
  }

  static std::string _get_next_key(const char *a, char separator, int start, int &end){
    _get_next(a,separator,start,end);
    std::string s(a+start,a+end);
    return s;
  }
  

  template<typename VALUE>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   std::map<std::string,VALUE> &get_){

    try {
      
      std::string keys_object_id = std::string("key_")+object_id;
      std::string values_object_id = std::string("values_")+object_id;
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();

      // keys are stored as a single string using ';' as separator
      std::string keys;
      get<std::string>(segment_id,
		       keys_object_id,
		       keys);

      // values are stored as fixed sized array
      std::pair<VALUE*, std::size_t> values = segment.find<VALUE>(values_object_id.c_str());

      int start=0;
      int end=0;
      for(int i=0;i<values.second;i++){
	std::string key = _get_next_key(keys.c_str(),
					_MAP_STRING_KEY_SEPARATOR,
					start,end);
	start=end+1;
	get_[key]=(values.first)[i];
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }

      
  }

  

  template<typename KEY, typename VALUE>
  void get(const std::string &segment_id,
	   const std::string &object_id,
	   std::map<KEY,VALUE> &get_){

    try {
      
      std::string keys_object_id = std::string("key_")+object_id;
      std::string values_object_id = std::string("values_")+object_id;
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      std::pair<KEY*, std::size_t> keys = segment.find<KEY>(keys_object_id.c_str());
      std::pair<VALUE*, std::size_t> values = segment.find<VALUE>(values_object_id.c_str());
      if(keys.second != get_.size()){
	throw shared_memory::Unexpected_size_exception(segment_id,
						       object_id,
						       keys.second, 
						       get_.size());
      }
      for(int i=0;i<keys.second;i++){
	get_[(keys.first)[i]]=(values.first)[i];
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }

      
  }

  
}
