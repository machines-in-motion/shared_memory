
template <class Serializable, int QUEUE_SIZE>
Exchange_manager_producer<Serializable,QUEUE_SIZE>::Exchange_manager_producer(std::string segment_id,
									      std::string object_id,
									      bool autolock,
									      bool clean_memory_on_exit )
  : segment_(bip::open_or_create, segment_id.c_str(), 100*65536),
    locker_(segment_id+"_locker",clean_memory_on_exit) {

  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  produced_ = segment_.find_or_construct<producer_queue>(object_id_producer_.c_str())();
  consumed_ = segment_.find_or_construct<consumer_queue>(object_id_consumer_.c_str())();
  segment_id_ = segment_id;
  consumer_started_ = false;
  values_ = new double[Serializable::serialization_size];
  clean_memory_on_exit_ = clean_memory_on_exit;
  autolock_ = autolock;
  
}

template<class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable,QUEUE_SIZE>::clean_memory( std::string segment_id ) {

    shared_memory::clear_shared_memory(segment_id);
    shared_memory::delete_segment(segment_id);
    
}

template <class Serializable, int QUEUE_SIZE>  
Exchange_manager_producer<Serializable,QUEUE_SIZE>::~Exchange_manager_producer(){

  delete[] values_;

  if (clean_memory_on_exit_){
    Exchange_manager_producer<Serializable,QUEUE_SIZE>::clean_memory(segment_id_);
  }
  
}


template<class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable,QUEUE_SIZE>::lock(){

  locker_.lock();

}


template<class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable,QUEUE_SIZE>::unlock(){

  locker_.unlock();

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable,QUEUE_SIZE>::set(const Serializable &serializable){

  if(autolock_){
    this->lock();
  }
  
  // transforming serializable to an array of double
  serializable.serialize(values_);

  // writing the serialized object in the shared memory
  for(int i=0;i<Serializable::serialization_size;i++){

    bool pushed = produced_->bounded_push(values_[i]);

    // memory overflow in the queue
    if(!pushed){

      if(autolock_){
	this->unlock();
      }
      
      throw Memory_overflow_exception("exchange_manager_producer: queue is full ");
    }
    
  }

  if(autolock_){
    this->unlock();
  }
  
}


template<class Serializable, int QUEUE_SIZE>
int Exchange_manager_producer<Serializable,QUEUE_SIZE>::get_last_consumed(){

  static int previous = -1;

  int value;

  if (consumed_->pop(value)){

    if (value!=previous){
      previous = value;
      return value;
    }

    else {
      return this->get_last_consumed();
    }
    
  }
  
  return -1;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable,QUEUE_SIZE>::get(std::deque<int> &get_consumed_ids){

  if(autolock_){
    this->lock();
  }
  
  int last_consumed = get_last_consumed();

  while(last_consumed>=0){

    get_consumed_ids.push_back(last_consumed);

    last_consumed = get_last_consumed();
    
  }

  if(autolock_){
    this->unlock();
  }
  
  return;

}
    
    








