/**
 * @file four_int_values.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief Demonstrate how to create a message with the exchange manager
 */
#include "shared_memory/demos/four_int_values.hpp"

namespace shared_memory {

  Four_int_values::Four_int_values()
    :values_(4) {}
  
  Four_int_values::Four_int_values(int a, int b, int c, int d)
    : values_(4) {
    id_= Four_int_values::next_id();
    values_[0]=a;
    values_[1]=b;
    values_[2]=c;
    values_[3]=d;
  }

  void Four_int_values::print() const {
    std::cout << id_ << " | ";
    for(int i=0;i<4;i++) std::cout << values_[i] << " ";
    std::cout << "\n";
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


  void Four_int_values::set_id(int id){
    this->id_ = id;
  }
}
