#pragma once

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/mutex.hpp"

namespace shared_memory
{


  template<class T>
  class Array
  {

  public:

    Array(std::string segment_id,
	  std::size_t size,
	  bool clear_on_destruction=false,
	  bool multiprocess_safe=true)
      : segment_id_(segment_id),
	clear_on_destruction_(clear_on_destruction),
	multiprocess_safe_(multiprocess_safe),
	item_size_(shared_memory::Serializer<T>::serializable_size()),
	size_(size),
	total_size_(item_size_*size),
	mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
    {


      segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								    segment_id.c_str(),
								    65536);


      shared_ = segment_manager_.find_or_construct<std::string>(segment_id.c_str())[size]();

      T t;
      const std::string& serialized = serializer_.serialize(t);
      for(std::size_t i=0;i<size_;i++)
	{
	  shared_[i]=serialized;
	  shared_[i].resize(serialized.size());
	}

    }
    

    ~Array()
    {
      if(clear_on_destruction_)
	{
	  Array<T>::clear(segment_id_); 
	}
    }


    T get(int index) 
    {
      if(index<0 || index>=size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      T t;
      serializer_.deserialize(shared_[index],t);
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
      return t;
    }

    void set(int index,const T& t) 
    {
      if(index<0 || index>=size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      const std::string& serialized = serializer_.serialize(t);
      shared_[index]=serialized;
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
    }
    
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
    std::string* shared_;
    T t_;
    
  };









}
