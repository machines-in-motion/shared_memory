

template<typename T>
array<T>::array(std::string segment_id,
		std::size_t size,
		bool clear_on_destruction,
		bool multiprocess_safe)
  : segment_id_(segment_id),
    clear_on_destruction_(clear_on_destruction),
    multiprocess_safe_(multiprocess_safe),
    size_(size),
    mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
{

  // fundamental type (e.g int, double, etc)
  // simple implementation
  if(std::is_fundamental<T>::value)
    {
      segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								    segment_id.c_str(),
								    size_*sizeof(T)+500);
      f_shared_ = segment_manager_.find_or_construct<T>(segment_id.c_str())[size_]();
      shared_ = nullptr;
    }

  // class instance, using cereal to serialize into char array
  else
    {
      
      item_size_ = shared_memory::Serializer<T>::serializable_size();
      total_size_ = item_size_*size;
  
      // note: I do not understand how the memory padding works for memory segment,
      // but using the exact required amount of memory does not work, hence the ugly +500.
      segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								    segment_id.c_str(),
								    total_size_*sizeof(char)+500);
      shared_ = segment_manager_.find_or_construct<char>(segment_id.c_str())[total_size_]();
      f_shared_ = nullptr;
    }

  
}
    

template<typename T>
array<T>::~array()
{
  if(clear_on_destruction_)
    {
      array<T>::clear(segment_id_); 
    }
}



template<typename T>
void array<T>::set(uint index,const T& t) 
{

  // fundamental type
  
  if(std::is_fundamental<T>::value)
    {
      if(index<0 || index>=size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      f_shared_[index]=t;
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
      return;
    }

  // serializable instance
  
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
      shared_[abs_index+index]=serialized[index];
    }

  if(multiprocess_safe_)
    {
      mutex_.unlock();
    }

}


template<typename T>
T array<T>::get(uint index) 
{

  // fundamental type
  
  if(std::is_fundamental<T>::value)
    {
      if(index<0 || index>=size_)
	{
	  throw std::runtime_error("invalid index");
	}
      if(multiprocess_safe_)
	{
	  mutex_.lock();
	}
      T r = f_shared_[index];
      if(multiprocess_safe_)
	{
	  mutex_.unlock();
	}
      return r;
    }
      

  // serializable instance
  
  uint abs_index = index*item_size_;

  if(abs_index<0 || abs_index>=total_size_)
    {
      throw std::runtime_error("invalid index");
    }

  if(multiprocess_safe_)
    {
      mutex_.lock();
    }
  
  serializer_.deserialize(std::string(&shared_[abs_index],item_size_),t_);

  if(multiprocess_safe_)
    {
      mutex_.unlock();
    }

  return t_;
  
}


    


