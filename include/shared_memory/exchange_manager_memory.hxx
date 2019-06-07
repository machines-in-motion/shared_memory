


template <class Serializable, int QUEUE_SIZE>
Exchange_manager_memory<Serializable,QUEUE_SIZE>::Exchange_manager_memory( std::string segment_id,
									   std::string object_id )

  : segment_(bip::open_or_create, segment_id.c_str(), 100*65536),
    locker_(std::string(segment_id+"_locker").c_str(),false) {

  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  object_id_status_ = object_id+"_status";
  produced_ = segment_.find_or_construct<producer_queue>(object_id_producer_.c_str())();
  consumed_ = segment_.find_or_construct<consumer_queue>(object_id_consumer_.c_str())();
  segment_id_ = segment_id;
  values_ = new double[Serializable::serialization_size];

  producer_size_ = 0;
  
}


template <class Serializable, int QUEUE_SIZE>
Exchange_manager_memory<Serializable,QUEUE_SIZE>::~Exchange_manager_memory(){

  unlock();
  delete[] values_;

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::set_status(Status status){

  lock();
  shared_memory::set<int>( segment_id_,
			   object_id_status_,
			   status );
  unlock();

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::get_status(Status &status){

  int r;
  
  lock();
  shared_memory::get<int>(segment_id_,
			  object_id_status_,
			  r);
  unlock();

  status = static_cast<Status>(r);
  
}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::clean(){

  Exchange_manager_memory<Serializable,QUEUE_SIZE>::clean_mutex(segment_id_);
  Exchange_manager_memory<Serializable,QUEUE_SIZE>::clean_memory(segment_id_);
  
}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::lock(){

  locker_.lock();

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::unlock(){

  locker_.unlock();

}


template<class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::clean_mutex( std::string segment_id ) {

  // mutex destruction called in destructor
  shared_memory::Mutex m(segment_id+"_locker",true);
    
}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::clean_memory( std::string segment_id ) {

  shared_memory::clear_shared_memory(segment_id);
  shared_memory::delete_segment(segment_id);

}


template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_memory<Serializable,QUEUE_SIZE>::read_serialized(Serializable &serializable) {

  int index=0;

  if ( produced_->pop(values_[index]) ) {

    for(index=1;index<Serializable::serialization_size;index++){

      bool poped = produced_->pop(values_[index]);

      if(!poped){

	throw std::runtime_error(std::string("half poped a serialized object !")+
				 std::string(" This is a bug in shared_memory/exchange_manager_memory.hxx\n"));
      }
      
    }
    
    serializable.create(values_);

    return true;
    
  }

  return false;

}


template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_memory<Serializable,QUEUE_SIZE>::write_serialized(const Serializable &serializable) {

  set_consumed_memory();
  
  if(producer_size_>=QUEUE_SIZE-1){
    return false;
  }
  
    // transforming serializable to an array of double
  serializable.serialize(values_);

  // writing the serialized object in the shared memory
  for(int i=0;i<Serializable::serialization_size;i++){

    bool pushed = produced_->bounded_push(values_[i]);

    if(!pushed){

      throw std::runtime_error(std::string("Failed to write a serialized object !")+
			       std::string(" This is a bug in shared_memory/exchange_manager_producer.hxx\n"));
    }

  }

  producer_size_ ++;
  return true;
}



template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::write_serialized_id(int id) {

  // if some consumed id could previously not be set in the shared memory
  // (because this latest was full), they have been buffered
  // in consumed_buffer_. Trying to purge it.
  
  while( ! consumed_buffer_.empty() ){
    int id = consumed_buffer_.front();
    bool pushed = consumed_->push(id);
    if(!pushed){
      break;
    } else {
      consumed_buffer_.pop_front();
    }
  }
  
  // trying to push id that was read into consumed_,
  // to inform the producer that this item has been consumed
  bool pushed = consumed_->push(id);
  
  // if consumed_ may be full, we buffer
  if(!pushed){
    consumed_buffer_.push_back(id);
  }
    
}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::set_consumed_memory() {

  int id;
  bool has_consumed;
  
  while(has_consumed){

      has_consumed = consumed_->pop(id);
      
      if (has_consumed) {

	producer_size_--;
	consumed_memory_.push_back(id);

      }
  }

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::get_consumed_ids(std::deque<int> &get_consumed_ids) {

  set_consumed_memory();

  while(! consumed_memory_.empty() ) {

    int id = consumed_memory_.front();
    get_consumed_ids.push_back(id);
    consumed_memory_.pop_front();
    
  }

}

