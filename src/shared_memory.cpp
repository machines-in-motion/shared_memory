#include "shared_memory/shared_memory.hpp"


namespace shared_memory {


  void clear_segment(const std::string &segment_id){
    boost::interprocess::shared_memory_object::remove(segment_id.c_str());
  }

  
  void clear_mutex(const std::string &object_id){
    boost::interprocess::named_mutex::remove(object_id.c_str());
  }

  
  void clear_mutexes(const std::vector<std::string> &mutexes){
    for(const std::string& str: mutexes){
      boost::interprocess::named_mutex::remove(str.c_str());
      std::string keys_object_id = std::string("key_")+str;
      std::string values_object_id = std::string("values_")+str;
      boost::interprocess::named_mutex::remove(keys_object_id.c_str());
      boost::interprocess::named_mutex::remove(values_object_id.c_str());
    }
  }

  
  void clear(const std::string &segment_id,
	     const std::vector<std::string> &mutexes){
    clear_mutexes(mutexes);
    clear_segment(segment_id);
  }
  
  void set(const std::string &segment_id,
	   const std::string &object_id,
	   const std::string &set_){

    set<char>(segment_id,
	      object_id,
	      set_.c_str(),
	      set_.size());
  }


  void get(const std::string &segment_id,
	   const std::string &object_id,
	   std::string &get_){
    
    get_.clear();
    
    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),_SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

      mutex.lock();
      std::pair<char*, std::size_t> object = segment.find<char>(object_id.c_str());
      for(int i=0;i<object.second;i++) get_+= (object.first)[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }

    
  }

    
}
