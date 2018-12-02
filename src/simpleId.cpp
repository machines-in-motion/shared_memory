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
