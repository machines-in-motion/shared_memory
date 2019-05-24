
template <class Serializable, int QUEUE_SIZE>
Exchange_manager_producer<Serializable,QUEUE_SIZE>::Exchange_manager_producer(std::string segment_id,
									      std::string object_id )
  : segment_(bip::open_or_create, segment_id.c_str(), 100*65536) {

  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  produced_ = segment_.find_or_construct<producer_queue>(object_id_producer_.c_str())();
  consumed_ = segment_.find_or_construct<consumer_queue>(object_id_consumer_.c_str())();
  segment_id_ = segment_id;
  consumer_started_ = false;
  values_ = new double[Serializable::serialization_size];

}

template <class Serializable, int QUEUE_SIZE>  
Exchange_manager_producer<Serializable,QUEUE_SIZE>::~Exchange_manager_producer(){

  delete[] values_;

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable,QUEUE_SIZE>::set(const Serializable &serializable){

  // transforming serializable to an array of double
  serializable.serialize(values_);

  // writing the serialized object in the shared memory
  for(int i=0;i<Serializable::serialization_size;i++){

    bool pushed = produced_->bounded_push(values_[i]);

    // memory overflow in the queue
    if(!pushed){
      throw Memory_overflow_exception("exchange_manager_producer: queue is full ");
    }
    
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

  int last_consumed = get_last_consumed();

  while(last_consumed>=0){

    get_consumed_ids.push_back(last_consumed);

    last_consumed = get_last_consumed();
    
  }

  return;

}
    
    








