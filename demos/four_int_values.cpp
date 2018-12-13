#include "shared_memory/demos/four_int_values.hpp"

namespace shared_memory {

  Four_int_values::Four_int_values(){}
  
  Four_int_values::Four_int_values(int a, int b, int c, int d){
    id_= Four_int_values::next_id();
    values_[0]=a;
    values_[1]=b;
    values_[2]=c;
    values_[3]=d;
  }

  static void prints(const double *serialized){
    for(int i=0;i<Four_int_values::serialization_size;i++){
      std::cout << serialized[i] << " ";
    }
    std::cout << "\n";
  }

  void Four_int_values::print() const {
    std::cout << id_ << " | ";
    for(int i=0;i<4;i++) std::cout << values_[i] << " ";
    std::cout << "\n";
  }
  
  void Four_int_values::create(double *serialized){
    id_ = static_cast<int>(serialized[0]);
    for(int i=0;i<4;i++){
      values_[i]=static_cast<int>(serialized[i+1]);
    }
  }

  void Four_int_values::serialize(double *serialized) const {
    serialized[0]=static_cast<double>(id_);
    for(int i=0;i<4;i++){
      serialized[i+1]=static_cast<double>(values_[i]);
    }
  }

  int Four_int_values::get_id() const {
    return id_;
  }

  int Four_int_values::next_id(){
    static int id = 0;
    id++;
    return id;
  }

  bool Four_int_values::equal(const Four_int_values &other) const {
    for(int i=0;i<4;i++){
      if (values_[i]!=other.values_[i]) return false;
    }
    return true;
  }

  bool Four_int_values::same(const Four_int_values &other) const {
    if(!this->equal(other)) return false;
    if (id_!=other.id_) return false;
    return true;
  }

  
}
