#pragma once

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/mutex.hpp"

namespace shared_memory
{


  template<class T>
  class array
  {

  public:

    array(std::string segment_id,
	  std::size_t size,
	  bool clear_on_destruction=false,
	  bool multiprocess_safe=true);


    ~array();
    
    T get(uint index);

    void set(uint index,const T& t);
    
    static void clear(std::string segment_id)
    {
      boost::interprocess::shared_memory_object::remove(segment_id.c_str());
      boost::interprocess::named_mutex::remove((segment_id+std::string("_mutex")).c_str());
    }
    

  private:

    boost::interprocess::managed_shared_memory segment_manager_;
    std::string segment_id_; // used as both segment and object id
    shared_memory::Serializer<T> serializer_;
    bool clear_on_destruction_;
    bool multiprocess_safe_;
    std::size_t item_size_;
    std::size_t size_;
    std::size_t total_size_;
    shared_memory::Mutex mutex_;
    char* shared_;
    T t_;
    
  };


  #include "array.hxx"








}
