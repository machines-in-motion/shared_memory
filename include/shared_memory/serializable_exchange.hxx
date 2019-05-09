

template<class Serializable>
Serializable_exchange<Serializable>::Serializable_exchange(std::string segment_id,
							   std::string object_id) :
  segment_id_(segment_id), object_id_(object_id) {

  data_ = new double[Serializable::serialization_size];
  for(int i=0;i<Serializable::serialization_size;i++) data_[i]=-1;
  shared_memory::set(segment_id_,object_id_,data_,Serializable::serialization_size);  
  
}


template<class Serializable>
Serializable_exchange<Serializable>::~Serializable_exchange(){}


template<class Serializable>
void Serializable_exchange<Serializable>::set(const Serializable &serializable){
  serializable.serialize(data_);
  shared_memory::set(segment_id_,object_id_,data_,Serializable::serialization_size);  
}


template<class Serializable>
void Serializable_exchange<Serializable>::read(Serializable &serializable){
  shared_memory::get(segment_id_,object_id_,data_,Serializable::serialization_size);
  serializable.create(data_);
}
