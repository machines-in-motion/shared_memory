/**
 * @file simpleId.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Implementation of the SimpleId class.
 */
#include "shared_memory/simpleId.hpp"

namespace shared_memory {

  SimpleId::SimpleId(){}

  SimpleId::SimpleId(int id):id_(id){}

  void SimpleId::create(double *serialized){
    id_ = static_cast<int>(serialized[0]);
  }

  void SimpleId::serialize(double *serialized) const{
    serialized[0] = static_cast<double>(id_);
  }

  int SimpleId::get_id() const { return id_; }

}
