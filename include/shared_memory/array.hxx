// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz


template<typename T, int SIZE>
array<T,SIZE>::array( std::string segment_id,
		      std::size_t size,
		      bool clear_on_destruction,
		      bool multiprocess_safe)
  : segment_id_(segment_id),
    size_(size),
    clear_on_destruction_(clear_on_destruction),
    multiprocess_safe_(multiprocess_safe),
    mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
{
  init(this->type_);
}

template<typename T, int SIZE>
array<T,SIZE>::~array()
{
  if(clear_on_destruction_)
    {
      clear_array(segment_id_);
    }
}

template<typename T, int SIZE>
void array<T,SIZE>::set(uint index, const T& t)
{
  set(index,t,this->type_);
}

template<typename T, int SIZE>
void array<T,SIZE>::get(uint index, T& t)
{
  get(index,t,this->type_);
}

void clear_array(std::string segment_id)
{
  boost::interprocess::shared_memory_object::remove(segment_id.c_str());
  boost::interprocess::named_mutex::remove((segment_id+std::string("_mutex")).c_str());
}

    
