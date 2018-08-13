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
  
    
}
