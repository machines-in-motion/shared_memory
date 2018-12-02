
template <class Serializable>
Exchange_manager_producer<Serializable>::Exchange_manager_producer(std::string segment_id,
								   std::string object_id,
								   int max_exchange_size)

  : items_(max_exchange_size) {

  segment_id_ = segment_id;
  object_id_producer_ = object_id+"_producer";
  object_id_consumer_ = object_id+"_consumer";
  object_id_reset_ = object_id+"_reset";
  
  previous_consumer_id_ = 0;

  consumer_started_ = false;
  
}

template <class Serializable>  
Exchange_manager_producer<Serializable>::~Exchange_manager_producer(){
  // informing the consumer that the producer stopped, giving it
  // a change to reset
  shared_memory::set(segment_id_,object_id_reset_,true);
}

template <class Serializable>  
void Exchange_manager_producer<Serializable>::clean_memory(){
    shared_memory::clear_shared_memory(segment_id_);
    shared_memory::delete_segment(segment_id_);
}

template <class Serializable>
bool Exchange_manager_producer<Serializable>::consumer_started() const {
  return consumer_started_;
}

template <class Serializable>
bool Exchange_manager_producer<Serializable>::set(const Serializable &serializable){
  bool added = items_.add(serializable);
  return added;
}


template <class Serializable>
void Exchange_manager_producer<Serializable>::update_memory(std::deque<int> *get_consumed_ids){

  
  // read shared memory to check if the consumer
  // did consume any data
  static double from_consumer[2];

  // format expected: [id, number of items consumed by consumer]
  int consumer_id = previous_consumer_id_;
  int nb_to_remove = 0;
  try {
    shared_memory::get(segment_id_,object_id_consumer_,from_consumer,2);
    consumer_id = static_cast<int>(from_consumer[0]);
    nb_to_remove = static_cast<int>(from_consumer[1]);
    consumer_started_ = true;
  } catch (shared_memory::Unexpected_size_exception &e) {
    // the memory has not been initialized yet
    consumer_started_=false;
  }

  if (consumer_started_) {
  
    bool should_remove_items = true;
    if (consumer_id == previous_consumer_id_) should_remove_items = false;
    if (consumer_id < 0) should_remove_items = false;
    if (nb_to_remove == 0) should_remove_items = false;

    if (should_remove_items){
      // the consumer did read some data, removing them
      // from the stack, and save their ids in get_consumed_ids
      previous_consumer_id_ = consumer_id;
      items_.remove(nb_to_remove,get_consumed_ids);
      shared_memory::set(segment_id_,object_id_producer_,
			 items_.get_data(),items_.get_data_size());
      // informing the consumer that we are aware these elements
      // have been consumed
      from_consumer[0]=-1.0;
      from_consumer[1]=static_cast<double>(nb_to_remove);
      shared_memory::set(segment_id_,object_id_consumer_,from_consumer,2);

      return;
    }

    // updating item stack in memory
    shared_memory::set(segment_id_,object_id_producer_,
		       items_.get_data(),items_.get_data_size());

  }

}

template <class Serializable>
void Exchange_manager_producer<Serializable>::update_memory(std::deque<int> &get_consumed_ids){
  this->update_memory(&get_consumed_ids);
}

template <class Serializable>
void Exchange_manager_producer<Serializable>::update_memory(){
  this->update_memory(NULL);
}







