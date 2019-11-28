// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz

#pragma once

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/mutex.hpp"

#include <cstring>

namespace shared_memory
{

  typedef std::integral_constant<int,0> SERIALIZABLE;
  typedef std::integral_constant<int,1> FUNDAMENTAL;
  typedef std::integral_constant<int,2> FUNDAMENTAL_ARRAY;
  
  namespace internal
  {

    template<typename T, int SIZE=0, typename Enable=void>
    class array_members
    {
    protected:
      Serializer<T> serializer_;
      char * shared_;
      std::size_t item_size_;
      std::size_t total_size_;
      SERIALIZABLE type_;
    };

    // defining members for serializable instances,
    // which will be stored as char arrays
    template<typename T, int SIZE>
    class array_members< T,
			 SIZE,
			 typename std::enable_if< std::is_fundamental<T>::value && SIZE!=0 >::type >
    {
    protected:
      T * shared_;
      std::size_t total_size_;
      FUNDAMENTAL_ARRAY type_;
    };


    // defining members for fundamental types
    // which will directly be stored
    template<typename T>
    class array_members< T ,
			 0,
			 typename std::enable_if< std::is_fundamental<T>::value >::type >
    {
    protected:
      T * shared_;
      FUNDAMENTAL type_;
    };
      
  }
  
  
  template<typename T, int SIZE=0>
  class array : public internal::array_members<T,SIZE>
  {

  private:

    // -----------------------------------------
    // implementation for serializable instances
    // -----------------------------------------

    void init( SERIALIZABLE )
    {
      this->item_size_ = Serializer<T>::serializable_size();
      this->total_size_ = this->item_size_*this->size_;
      // note: I do not understand how the memory padding works for memory segment,
      // but using the exact required amount of memory does not work, hence the ugly +500.
      segment_manager_ =
	boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
						   segment_id_.c_str(),
						   this->total_size_*sizeof(char)+500);
      this->shared_ =
	  segment_manager_.find_or_construct<char>(segment_id_.c_str())[this->total_size_]();

    }

    void set(uint index,const T& t, SERIALIZABLE) 
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

    void get(uint index, T& t, SERIALIZABLE) 
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
      this->serializer_.deserialize(std::string(&this->shared_[abs_index],
						this->item_size_),t);
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
    }

    // ----------------------------------------------
    // implementation for arrays of fundamental types
    // ----------------------------------------------

    void init( FUNDAMENTAL_ARRAY )
    {
      this->total_size_ = this->item_size_*SIZE;
      // note: I do not understand how the memory padding works for memory segment,
      // but using the exact required amount of memory does not work, hence the ugly +500.
      segment_manager_ =
	boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
						   segment_id_.c_str(),
						   this->total_size_*sizeof(T)+500);
      this->shared_ =
	segment_manager_.find_or_construct<char>(segment_id_.c_str())[this->total_size_]();

      
    }

    void set(uint index, const T& t, FUNDAMENTAL_ARRAY) 
    {

      uint abs_index = index*SIZE;
      if(abs_index<0 || index>=this->total_size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      std::memcpy(this->shared_+sizeof(T)*abs_index,
		  &t,
		  SIZE);
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
    }

    void get(uint index,T& t,FUNDAMENTAL_ARRAY) 
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
      std::memcpy(t,
		  this->shared_+sizeof(T)*abs_index,
		  SIZE);
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
    }

    
    // ------------------------------------
    // implementation for fundamental types
    // ------------------------------------

    void init( FUNDAMENTAL )
    {
      segment_manager_ =
	boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
						   segment_id_.c_str(),
						   size_*sizeof(T)+500);
      this->shared_ =
	segment_manager_.find_or_construct<T>(segment_id_.c_str())[size_]();
    }

    void set(uint index,const T& t, FUNDAMENTAL) 
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
    
    void get(uint index, T& t, FUNDAMENTAL) 
    {
      if(index<0 || index>=size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      t = this->shared_[index];
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
    }


    
  public:

    array( std::string segment_id,
	   std::size_t size,
	   bool clear_on_destruction=true,
	   bool multiprocess_safe=true)
      : segment_id_(segment_id),
	size_(size),
	clear_on_destruction_(clear_on_destruction),
	multiprocess_safe_(multiprocess_safe),
	mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
    {
      init(this->type_);
    }

    ~array()
    {
      if(clear_on_destruction_)
	{
	  array<T>::clear(segment_id_); 
	}
    }

    void set(uint index, const T& t)
    {
      set(index,t,this->type_);
    }

    void get(uint index, T& t)
    {
      get(index,t,this->type_);
    }

    static void clear(std::string segment_id)
    {
      boost::interprocess::shared_memory_object::remove(segment_id.c_str());
      boost::interprocess::named_mutex::remove((segment_id+std::string("_mutex")).c_str());
    }

    
  private:

    boost::interprocess::managed_shared_memory segment_manager_;
    std::string segment_id_;
    std::size_t size_;
    bool clear_on_destruction_;
    bool multiprocess_safe_;
    shared_memory::Mutex mutex_;
    
  };

}
