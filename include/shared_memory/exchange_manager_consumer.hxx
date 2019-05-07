


template <class Serializable>
Exchange_manager_consumer<Serializable>::Exchange_manager_consumer(std::string segment_id,
								   std::string object_id)


  : segment_(bip::open_or_create, segment_id.c_str(), 40*65536) {

  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  produced_ = segment_.find_or_construct<producer_ring>(object_id_producer_.c_str())();
  consumed_ = segment_.find_or_construct<consumer_ring>(object_id_consumer_.c_str())();
  segment_id_ = segment_id;
  values_ = new double[Serializable::serialization_size];
  previous_consumed_id_=-1;
  
}


template <class Serializable>
Exchange_manager_consumer<Serializable>::~Exchange_manager_consumer(){

  delete[] values_;
  this->clean_memory();

}


template <class Serializable>  
void Exchange_manager_consumer<Serializable>::clean_memory(){
  boost::interprocess::shared_memory_object::remove(segment_id_.c_str());
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

