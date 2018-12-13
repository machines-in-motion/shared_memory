

template <class Serializable>
Serializable_stack<Serializable>::Serializable_stack(int nb_max_items){

  // nb of doubles required to serialize an item
  items_serialization_size_ = Serializable::serialization_size;

  // number of items this container will be able to hold without memory overflow
  items_max_numbers_ = nb_max_items;

  // total number of doubles the memory buffer should have 
  array_size_ = 2 + nb_max_items*items_serialization_size_;
  data_ = new double[array_size_];

  // current number of items holded by the container
  nb_items_ = 0;

  // id that increases everytime an item is added
  id_ = 0;

  // first index of data_ is reserved for id
  data_[0] = id_;

  // second index reserved for number of items currently holded
  data_[1] = nb_items_;

}


template <class Serializable>
Serializable_stack<Serializable>::~Serializable_stack(){
  delete[] data_;
}


// return true if full after addition
template <class Serializable>
bool Serializable_stack<Serializable>::add(const Serializable &serializable){

  if (nb_items_>=items_max_numbers_){
    throw std::overflow_error("serializable stack: memory overflow when setting a new item");
  }
  
  int data_index = 2+nb_items_*items_serialization_size_;

  serializable.serialize(&(data_[data_index]));

  nb_items_++;
  
  data_[0]++;
  data_[1]=nb_items_;



  if (nb_items_==items_max_numbers_) return true;

  return false;
}


template <class Serializable>
void Serializable_stack<Serializable>::remove(int nb_items, std::deque<int> *get_removed_ids){

  // we can not remove more items than currently contained
  if(nb_items>nb_items_) nb_items = nb_items_;

  if(nb_items==0) return;
  
  // saving ids of items that will be removed
  for(int i=0;i<nb_items;i++){
    int id_index = 2+i*items_serialization_size_+Serializable::serialization_id_index;
    int id = static_cast<int>(data_[id_index]);
    if (get_removed_ids){
      get_removed_ids->push_back(id);
    }
  }

  nb_items_ -= nb_items;

  // shifting the full memory to the left, over what was holding items to delete
  std::memmove( data_+2,
	       data_+2+nb_items*items_serialization_size_,
	       nb_items_ * items_serialization_size_ * sizeof(double));


  // updating data with new current number of items
  data_[0]++;
  data_[1] = nb_items_;
  
}


template <class Serializable>
void Serializable_stack<Serializable>::remove(int nb_items, std::deque<int> &get_removed_ids){
  this->remove(nb_items,&get_removed_ids);
  
}

template <class Serializable>
void Serializable_stack<Serializable>::remove(int nb_items){
  this->remove(nb_items,NULL);
}


template <class Serializable>
double * const Serializable_stack<Serializable>::get_data(){
  return data_;
}

template <class Serializable>
int Serializable_stack<Serializable>::get_data_size(){
  return array_size_;
}



///////////////////////
///////////////////////
///////////////////////


template <class Serializable>
Serializable_stack_reader<Serializable>::Serializable_stack_reader(int max_items){
  array_size_ = 2+Serializable::serialization_size*max_items;
  data_ = new double[array_size_];
  index_ = 0;
}

template <class Serializable>
Serializable_stack_reader<Serializable>::~Serializable_stack_reader(){
  delete[] data_;
}

template <class Serializable>
double * const Serializable_stack_reader<Serializable>::get_data_and_reset(){
  this->reset();
  return data_;
}

template <class Serializable>
double * const Serializable_stack_reader<Serializable>::get_data(){
  return data_;
}

template <class Serializable>
void Serializable_stack_reader<Serializable>::reset(){
  index_ = 0;
}

template <class Serializable>
void Serializable_stack_reader<Serializable>::reset(int nb_removed){
  index_ -= nb_removed;
}


template <class Serializable>
int Serializable_stack_reader<Serializable>::get_id(){
  return data_[0];
}

template <class Serializable>
int Serializable_stack_reader<Serializable>::get_index(){
  return index_;
}

template <class Serializable>
bool Serializable_stack_reader<Serializable>::empty(){
  int nb_items = data_[1];
  if(index_>=nb_items){
    return true;
  }
  return false;
}

template <class Serializable>
void Serializable_stack_reader<Serializable>::read(Serializable &get){
  // ! assumes not empty
  int index_read = 2+Serializable::serialization_size * index_ ;
  get.create(&(data_[index_read]));
  index_++;
}

template <class Serializable>
int Serializable_stack_reader<Serializable>::get_data_size(){
  return array_size_;
}




