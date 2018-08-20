#include "shared_memory/exceptions.h"

namespace shared_memory {

  
  Allocation_exception::Allocation_exception(const std::string &segment_id,
					     const std::string &object_id){
    std::ostringstream s;
    s <<  "shared_memory : ran out of memory when allocating " <<
      segment_id << " ("<< object_id << ")";
    this->error_message_ = s.str();
  }


  Allocation_exception::~Allocation_exception() throw () {}


  const char * Allocation_exception::what () const throw () {
    return this->error_message_.c_str();
  }



  Unexpected_size_exception::Unexpected_size_exception(const std::string &segment_id,
						       const std::string &object_id,
						       int expected_size,
						       int size_given){
    std::ostringstream s;
    s <<  "shared_memory : size error when setting/getting " <<
      segment_id << " ("<< object_id << "): expected size: " << expected_size <<
      " provided size: " << size_given; 
    this->error_message_ = s.str();

  }


  Unexpected_size_exception::~Unexpected_size_exception() throw () {}


  const char * Unexpected_size_exception::what () const throw () {
    return this->error_message_.c_str();
  }

  
  
}

