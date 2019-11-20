

template<class T>
array<T>::array(std::string segment_id,
		std::size_t size,
		bool clear_on_destruction,
		bool multiprocess_safe,
		uint memory_size)
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
								memory_size);

  std::cout << "all ok: " << segment_manager_.check_sanity() << "\n";
  std::cout << "size: " << segment_manager_.get_size() << "\n";
  std::cout << "free memory: " << segment_manager_.get_free_memory() << "\n";
  std::cout << "num named objects: " << segment_manager_.get_num_named_objects() << "\n";
  std::cout << "num unique objects: " << segment_manager_.get_num_unique_objects() << "\n";


  if(clear_on_destruction)
    {
      shared_ = segment_manager_.find_or_construct<std::string>(segment_id.c_str())[size]();
      T t;
      const std::string& serialized = serializer_.serialize(t);
      for(std::size_t i=0;i<size_;i++)
	{
	  shared_[i]=serialized;
	  shared_[i].resize(serialized.size());
	}
    }

  else
    {
      std::pair<std::string*,std::size_t> ret = segment_manager_.find<std::string>(segment_id.c_str());
      shared_ = ret.first;
      if(ret.second!=size)
	{
	  throw std::runtime_error("shared memory array created with unexpected size");
	}
      std::cout << "found with size: " << ret.second << "\n";
    }

  std::cout << "----\n";
  std::cout << "first element:\n";
  std::cout << shared_[0] << "\n";
  std::cout << "----\n";
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


template<class T>
T array<T>::get(uint index) 
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


    


