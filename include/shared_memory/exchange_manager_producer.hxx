
template <class Serializable>
Exchange_manager_producer<Serializable>::Exchange_manager_producer(std::string segment_id,
								   std::string object_id,
								   bool exception_if_not_consumed)
  : segment_(bip::open_or_create, segment_id.c_str(), 40*65536) {

  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  produced_ = segment_.find_or_construct<producer_queue>(object_id_producer_.c_str())();
  consumed_ = segment_.find_or_construct<consumer_queue>(object_id_consumer_.c_str())();
  segment_id_ = segment_id;
  consumer_started_ = false;
  values_ = new double[Serializable::serialization_size];
  exception_if_not_consumed_=exception_if_not_consumed;
  if (exception_if_not_consumed_){
    produced_ids_ = new std::queue<int>();
  }
  
}

template <class Serializable>  
Exchange_manager_producer<Serializable>::~Exchange_manager_producer(){
  // informing the consumer that the producer stopped, giving it
  // a change to reset
  delete[] values_;
  if (exception_if_not_consumed_){
    delete produced_ids_;
  }
}


template <class Serializable>
void Exchange_manager_producer<Serializable>::set(const Serializable &serializable){
  serializable.serialize(values_);
  for(int i=0;i<Serializable::serialization_size;i++){
    produced_->push(values_[i]);
  }
  if (exception_if_not_consumed_){
    produced_ids_->push(serializable.get_id());
  }
}


template<class Serializable>
int Exchange_manager_producer<Serializable>::get_last_consumed(){
  static int previous = -1;
  int value;
  if (consumed_->pop(value)){
    if (value!=previous){
      previous = value;
      return value;
    } else {
      return this->get_last_consumed();
    }
  }
  return -1;
}

template <class Serializable>
bool Exchange_manager_producer<Serializable>::get(std::deque<int> &get_consumed_ids){

  int last_consumed = get_last_consumed();

  while(last_consumed>=0){

    if (exception_if_not_consumed_){
      if(last_consumed != produced_ids_->front()){
	throw Not_consumed_exception(produced_ids_->front());
      }  else {
	produced_ids_->pop();
      }
    }

    get_consumed_ids.push_back(last_consumed);

    last_consumed = get_last_consumed();
    
  }

  return produced_ids_->empty();

}
    
    








