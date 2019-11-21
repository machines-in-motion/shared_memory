

template<class T>
array<T>::array(std::string segment_id,
		std::size_t size,
		bool clear_on_destruction,
		bool multiprocess_safe)
  : segment_id_(segment_id),
    clear_on_destruction_(clear_on_destruction),
    multiprocess_safe_(multiprocess_safe),
    item_size_(shared_memory::Serializer<T>::serializable_size()),
    size_(size),
    total_size_(item_size_*size),
    mutex_(segment_id+std::string("_mutex"),clear_on_destruction)
{

  // note: I do not understand how the memory padding works for memory segment,
  // but using the exact required amount of memory does not work, hence the ugly +500.
  segment_manager_ = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
								segment_id.c_str(),
								total_size_*sizeof(char)+500);

  try
    {
      shared_ = segment_manager_.find_or_construct<char>(segment_id.c_str())[total_size_]();
    } catch (...)
    {
      std::pair<char*,std::size_t> ret = segment_manager_.find<char>(segment_id.c_str());
      shared_ = ret.first;
    }

  
}
    

template<class T>
array<T>::~array()
{
  if(clear_on_destruction_)
    {
      array<T>::clear(segment_id_); 
    }
}



template<class T>
void array<T>::set(uint index,const T& t) 
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
      shared_[abs_index+index]=serialized[index];
    }

  if(multiprocess_safe_)
    {
      mutex_.unlock();
    }

}


template<class T>
T array<T>::get(uint index) 
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
  
  serializer_.deserialize(std::string(&shared_[abs_index],item_size_),t_);

  if(multiprocess_safe_)
    {
      mutex_.unlock();
    }

  return t_;
}


    


