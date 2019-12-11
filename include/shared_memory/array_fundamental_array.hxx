// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz


template<typename T, int SIZE>
void array<T,SIZE>::init( FUNDAMENTAL_ARRAY )
{
  this->total_size_ = size_*SIZE;

  
  uint segment_size = get_segment_size(this->total_size_,sizeof(T));

  segment_manager_ =
    boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
					       segment_id_.c_str(),
					       segment_size);
  this->shared_ =
    segment_manager_.find_or_construct<T>(segment_id_.c_str())[this->total_size_]();
}


template<typename T, int SIZE>
void array<T,SIZE>::set(uint index, const T& t, FUNDAMENTAL_ARRAY) 
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
	      sizeof(T)*SIZE);

  if(multiprocess_safe_)
    {
      mutex_.unlock();
    }
}


template<typename T, int SIZE>
void array<T,SIZE>::get(uint index,T& t,FUNDAMENTAL_ARRAY) 
{
  uint abs_index = index*SIZE;
  if(abs_index<0 || abs_index>=this->total_size_)
    {
      throw std::runtime_error("invalid index");
    }
  if(multiprocess_safe_)
    {
      mutex_.lock();
    }
  std::memcpy(&t,
  	      this->shared_+sizeof(T)*abs_index,
  	      sizeof(T)*SIZE);
  if(multiprocess_safe_)
    {
      mutex_.unlock();
    }
}
