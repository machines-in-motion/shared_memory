#pragma once

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/mutex.hpp"

namespace shared_memory
{

  namespace internal
  {
    
    template<typename T, typename Enable=void>
    class array_members;

    // defining members for fundamental types
    // which will directly be stored
    template<typename T>
    class array_members< T , typename std::enable_if<std::is_fundamental<T>::value>::type > 
    {
    protected:
      T * shared_;
    };
      
    // defining members for serializable instances,
    // which will be stored as char arrays
    template<typename T>
    class array_members< T, typename std::enable_if<!std::is_fundamental<T>::value>::type >
    {
    protected:
      shared_memory::Serializer<T> serializer_;
      char * shared_;
      std::size_t item_size_;
      std::size_t total_size_;
    };
      
  }
  
  
  template<typename T, uint SIZE=0>
  class array : public internal::array_members<T>
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
	clear_on_destruction_(clear_on_destruction),
	multiprocess_safe_(multiprocess_safe),
	mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
    {
      segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								    segment_id.c_str(),
								    size_*sizeof(T)+500);
      this->shared_ = segment_manager_.find_or_construct<T>(segment_id.c_str())[size_]();

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
      T r = this->shared_[index];
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
      this->shared_[index]=t;
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
	clear_on_destruction_(clear_on_destruction),
	multiprocess_safe_(multiprocess_safe),
	mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
    {
      this->item_size_ = shared_memory::Serializer<T>::serializable_size();
      this->total_size_ = this->item_size_*this->size_,
      // note: I do not understand how the memory padding works for memory segment,
      // but using the exact required amount of memory does not work, hence the ugly +500.
      segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								    segment_id.c_str(),
								    this->total_size_*sizeof(char)+500);
      this->shared_ = segment_manager_.find_or_construct<char>(segment_id.c_str())[this->total_size_]();
    }

  
    template<typename Q=T>
    typename std::enable_if<!std::is_fundamental<Q>::value, void>::type
    set(uint index,const T& t) 
    {

      uint abs_index = index*this->item_size_;
      if(abs_index<0 || index>=this->total_size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      const std::string& serialized = this->serializer_.serialize(t);
      for(uint index=0;index<this->item_size_;index++)
	{
	  this->shared_[abs_index+index]=serialized[index];
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
      uint abs_index = index*this->item_size_;
      if(abs_index<0 || abs_index>=this->total_size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      this->serializer_.deserialize(std::string(&this->shared_[abs_index],this->item_size_),t_);
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

    std::string segment_id_; // used as both segment and object id
    std::size_t size_;
    bool clear_on_destruction_;
    bool multiprocess_safe_;
    shared_memory::Mutex mutex_;
    boost::interprocess::managed_shared_memory segment_manager_;
    T t_;

  };

}
