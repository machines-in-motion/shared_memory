


template <class Serializable, int QUEUE_SIZE>
Exchange_manager_consumer<Serializable,QUEUE_SIZE>::Exchange_manager_consumer(std::string segment_id,
									      std::string object_id,
									      bool autolock,
									      bool clean_memory)


  : segment_(bip::open_or_create, segment_id.c_str(), 100*65536),
    locker_(segment_id+"_locker",clean_memory) {

  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  produced_ = segment_.find_or_construct<producer_queue>(object_id_producer_.c_str())();
  consumed_ = segment_.find_or_construct<consumer_queue>(object_id_consumer_.c_str())();
  segment_id_ = segment_id;
  values_ = new double[Serializable::serialization_size];
  previous_consumed_id_=-1;
  clean_memory_ = clean_memory;
  autolock_ = autolock;
  
}


template <class Serializable, int QUEUE_SIZE>
Exchange_manager_consumer<Serializable,QUEUE_SIZE>::~Exchange_manager_consumer(){
  
  delete[] values_;
  if(clean_memory_){
    Exchange_manager_consumer<Serializable,QUEUE_SIZE>::clean_memory(segment_id_);
  }

}


// mutex should clean its own memory upon destruction, but this may not happen if there
// have been a bug somewhere and the program crashed without proper cleanup
template<class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable,QUEUE_SIZE>::clean_mutex( std::string segment_id ) {

  shared_memory::Mutex m(segment_id+"_locker",true);
    
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable,QUEUE_SIZE>::clean_memory( std::string segment_id ) {

  shared_memory::clear_shared_memory(segment_id);
  shared_memory::delete_segment(segment_id);

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable,QUEUE_SIZE>::lock(){

  locker_.lock();
}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable,QUEUE_SIZE>::unlock(){

  locker_.unlock();
}


template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_consumer<Serializable,QUEUE_SIZE>::consume(Serializable &serializable,
								 bool lock){

  if (lock && autolock_){
    locker_.lock();
  }
  
  int index=0;

  if ( produced_->pop(values_[index]) ) {

    for(index=1;index<Serializable::serialization_size;index++){

      bool poped = produced_->pop(values_[index]);

      if(!poped){

	if(autolock_){
	  this->unlock();
	}

	throw std::runtime_error(std::string("half poped a serialized object !")+
				 std::string(" This is a bug in shared_memory/exchange_manager_consumer.hxx\n"));
      }
      
    }
    serializable.create(values_);

    int id = serializable.get_id();

    if(id==previous_consumed_id_){

      bool r = this->consume(serializable,false); // false: already locked
      
      if(lock && autolock_){
	locker_.unlock();
      }
      
      return r;
    }

    previous_consumed_id_ = id;
    consumed_->push(id);

    if(lock && autolock_){
      locker_.unlock();
    }

    return true;
    
  } else {

    if(lock && autolock_){
      locker_.unlock();
    }
    
    return false;
    
  }

  
}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable,QUEUE_SIZE>::purge(){

  int foo;

  while (true){
    bool poped = produced_->pop(foo);
    if(!poped){
      break;
    }
  }

  while (true){
    bool poped = consumed_->pop(foo);
    if(!poped){
      break;
    }
  }
  
}

