#include "shared_memory/shared_memory.hpp"


namespace shared_memory {

  static std::set<std::string> _SEGMENTS;
  static std::set<std::string> _MUTEXES;

  void _add_segment(const std::string &segment_id){
    _SEGMENTS.insert(segment_id);
  }
  
  void _add_mutex(const std::string &object_id){
    _MUTEXES.insert(object_id);
  }

  void _add(const std::string &segment_id,
	    const std::string &object_id){
    _add_segment(segment_id);
    _add_mutex(object_id);
  }

  
  void _clear_all_mutexes(){
    for(const std::string& object_id : _MUTEXES){
      boost::interprocess::named_mutex::remove(object_id.c_str());
    }
    _MUTEXES.clear();
  }

  void _clear_all_segments(){
    for(const std::string& segment_id : _SEGMENTS){
      boost::interprocess::shared_memory_object::remove(segment_id.c_str());
    }
    _SEGMENTS.clear();
  }

  void clear_mutexes(const std::vector<std::string> &mutexes){
    for(const std::string& str: mutexes){
      boost::interprocess::named_mutex::remove(str.c_str());
    }
  }
  
  void clear(){
    _clear_all_mutexes();
    _clear_all_segments();
  }


  class Cleaner {
  public:
    ~Cleaner(){
      clear();
    }
  };

  static Cleaner cleaner;

    
}
