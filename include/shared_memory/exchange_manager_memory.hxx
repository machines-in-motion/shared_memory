

template<class Serializable>
Serialized_read<Serializable>::Serialized_read():size_(0){
  values_ = new double[Serializable::serialization_size];
}

template<class Serializable>
Serialized_read<Serializable>::~Serialized_read(){
  delete[] values_;
}

template<class Serializable>
void Serialized_read<Serializable>::set(double value){
  buffer_.push_back(value);
  size_++;
}

template<class Serializable>
bool Serialized_read<Serializable>::read(Serializable &serializable){
  if (size_>=Serializable::serialization_size){
    for(int i=0;i<Serializable::serialization_size;i++){
      values_[i]=buffer_.front();
      buffer_.pop_front();
      size_--;
    }
    serializable.create(values_);
    return true;
  }
  return false;
}


template<class Serializable>
Serialized_write<Serializable>::Serialized_write() {
  values_ = new double[Serializable::serialization_size];
}

template<class Serializable>
Serialized_write<Serializable>::~Serialized_write(){
  delete[] values_;
}

template<class Serializable>
bool Serialized_write<Serializable>::empty(){
  return buffer_.empty();
}

template<class Serializable>
double Serialized_write<Serializable>::front(){
  double value = buffer_.front();
  return value;
}

template<class Serializable>
void Serialized_write<Serializable>::pop(){
  buffer_.pop_front();
}

template<class Serializable>
bool Serialized_write<Serializable>::write(const Serializable &serializable){
  serializable.serialize(values_);
  for(int i=0;i<Serializable::serialization_size;i++){
    buffer_.push_back(values_[i]);
  }
}




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

  while (true){

    double value;
    bool poped = produced_->pop(value);
    if (poped) {
      serialized_read_.set(value);
    } else {
      break;
    }

  }

  bool read = serialized_read_.read(serializable);
  
  return read;

}


template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::write_serialized(const Serializable &serializable) {
  
  serialized_write_.write(serializable);

  while(! serialized_write_.empty() ){

    double value = serialized_write_.front();
    bool pushed = produced_->bounded_push( value );

    if(pushed) {
      serialized_write_.pop();
    } else {
      break;
    }
    
  }

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
void Exchange_manager_memory<Serializable,QUEUE_SIZE>::get_consumed_ids(std::deque<int> &get_consumed_ids) {

  int id;
  bool has_consumed;
  
  while(has_consumed){

      has_consumed = consumed_->pop(id);
      
      if (has_consumed) {

	get_consumed_ids.push_back(id);

      }
  }
  
}

