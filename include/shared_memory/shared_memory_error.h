#pragma once

#include <iostream>
#include <exception>
#include <sstream>
#include <string>

namespace shared_memory {


  class Shared_memory_allocation_exception : public std::exception {

  public :

    Shared_memory_allocation_exception(const std::string &segment_id,
				       const std::string &object_id);
    ~Shared_memory_allocation_exception() throw () {}
    const char * what () const throw ();

  private:

    std::string error_message_;

  };


}
