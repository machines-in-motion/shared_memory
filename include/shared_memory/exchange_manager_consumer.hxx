


template <class Serializable>
Exchange_manager_consumer<Serializable>::Exchange_manager_consumer(std::string segment_id,
								   std::string object_id,
								   bool clean_memory)


  : segment_(bip::open_or_create, segment_id.c_str(), 100*65536) {

  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  produced_ = segment_.find_or_construct<producer_queue>(object_id_producer_.c_str())();
  consumed_ = segment_.find_or_construct<consumer_queue>(object_id_consumer_.c_str())();
  segment_id_ = segment_id;
  values_ = new double[Serializable::serialization_size];
  previous_consumed_id_=-1;
  clean_memory_ = clean_memory;
  
}


template <class Serializable>
Exchange_manager_consumer<Serializable>::~Exchange_manager_consumer(){

  delete[] values_;
  if(clean_memory_){
    this->clean_memory();
  }

}


template <class Serializable>  
void Exchange_manager_consumer<Serializable>::clean_memory(){
  shared_memory::clear_shared_memory(segment_id_);
  shared_memory::delete_segment(segment_id_);
}



template <class Serializable>
bool Exchange_manager_consumer<Serializable>::consume(Serializable &serializable){

  int index=0;

  if ( produced_->pop(values_[index]) ) {

    for(index=1;index<Serializable::serialization_size;index++){
      produced_->pop(values_[index]);
    }
    serializable.create(values_);

    int id = serializable.get_id();

    if(id==previous_consumed_id_){
      return this->consume(serializable);
    }

    previous_consumed_id_ = id;
    consumed_->push(id);
    
    return true;
    
  } else {
    return false;
  }
  
}

