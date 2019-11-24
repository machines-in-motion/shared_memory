#pragma once

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/mutex.hpp"

namespace shared_memory
{


  template<typename T>
  class array
  {

  public:

    //
    // version for fundamental types (int, double, char, etc) --------------
    //
    
    template<typename Q=T>
    array(std::string segment_id,
	  std::size_t size,
	  bool clear_on_destruction=true,
	  bool multiprocess_safe=true,
	  typename std::enable_if<std::is_fundamental<Q>::value>::type* = 0)
      : segment_id_(segment_id),
	size_(size),
	item_size_(-1),
	total_size_(-1),
	clear_on_destruction_(clear_on_destruction),
	multiprocess_safe_(multiprocess_safe),
	mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
    {
      segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								    segment_id.c_str(),
								    size_*sizeof(T)+500);
      shared_ = segment_manager_.find_or_construct<T>(segment_id.c_str())[size_]();

    }

    template<typename Q=T>
    typename std::enable_if<std::is_fundamental<Q>::value, Q>::type
    get(uint index) 
    {
      if(index<0 || index>=size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      T r = shared_[index];
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
      return r;
    }

    template<typename Q=T>
    typename std::enable_if<std::is_fundamental<Q>::value, void>::type
    set(uint index,const T& t) 
    {
      if(index<0 || index>=size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      shared_[index]=t;
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
      return;
    }

    
  public:

    //
    // version for serializable instances (implement cereal's serialiable) --------------
    //

    template<typename Q=T>
    array(std::string segment_id,
	  std::size_t size,
	  bool clear_on_destruction=true,
	  bool multiprocess_safe=true,
	  typename std::enable_if<!std::is_fundamental<Q>::value>::type* = 0)
      : segment_id_(segment_id),
	size_(size),
	item_size_(shared_memory::Serializer<T>::serializable_size()),
	total_size_(item_size_*size_),
	clear_on_destruction_(clear_on_destruction),
	multiprocess_safe_(multiprocess_safe),
	mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
    {
      // note: I do not understand how the memory padding works for memory segment,
      // but using the exact required amount of memory does not work, hence the ugly +500.
      segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								    segment_id.c_str(),
								    total_size_*sizeof(char)+500);
      i_shared_ = segment_manager_.find_or_construct<char>(segment_id.c_str())[total_size_]();
    }

  
    template<typename Q=T>
    typename std::enable_if<!std::is_fundamental<Q>::value, void>::type
    set(uint index,const T& t) 
    {

      uint abs_index = index*item_size_;
      if(abs_index<0 || index>=total_size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      const std::string& serialized = serializer_.serialize(t);
      for(uint index=0;index<item_size_;index++)
	{
	  i_shared_[abs_index+index]=serialized[index];
	}
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
    }


    template<typename Q=T>
    typename std::enable_if<!std::is_fundamental<Q>::value, Q>::type
    get(uint index) 
    {
      uint abs_index = index*item_size_;
      if(abs_index<0 || abs_index>=total_size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      serializer_.deserialize(std::string(&i_shared_[abs_index],item_size_),t_);
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
      return t_;
    }

    
  public:

    //
    // common code ---------------------
    //
    
    
    ~array()
    {
      if(clear_on_destruction_)
	{
	  array<T>::clear(segment_id_); 
	}
    }
    
    static void clear(std::string segment_id)
    {
      boost::interprocess::shared_memory_object::remove(segment_id.c_str());
      boost::interprocess::named_mutex::remove((segment_id+std::string("_mutex")).c_str());
    }
    

  private:

    shared_memory::Serializer<T> serializer_;
    std::string segment_id_; // used as both segment and object id
    std::size_t size_;
    std::size_t item_size_;
    std::size_t total_size_;
    bool clear_on_destruction_;
    bool multiprocess_safe_;
    shared_memory::Mutex mutex_;
    boost::interprocess::managed_shared_memory segment_manager_;
    char* i_shared_;
    T* shared_;
    T t_;

  };

}
