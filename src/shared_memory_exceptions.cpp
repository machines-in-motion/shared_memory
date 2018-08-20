#include "shared_memory/exceptions.h"

namespace shared_memory {

  Allocation_exception::Allocation_exception(const std::string &segment_id,
					     const std::string &object_id){
    std::ostringstream s;
    s <<  "shared_memory : ran out of memory when allocating " << segment_id << "("<< object_id << ")";
    this->error_message_ = s.str();
  }


  Allocation_exception::~Allocation_exception() throw () {}


  const char * Allocation_exception::what () const throw () {
    return this->error_message_.c_str();
  }


}
