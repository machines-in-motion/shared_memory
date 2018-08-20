#pragma once

#include <iostream>
#include <exception>
#include <sstream>
#include <string>

namespace shared_memory {


  class Allocation_exception : public std::exception {

  public :

    Allocation_exception(const std::string &segment_id,
			 const std::string &object_id);
    ~Allocation_exception() throw ();
    const char * what () const throw ();

  private:

    std::string error_message_;

  };


  class Unexpected_size_exception : public std::exception {

  public :

    Unexpected_size_exception(const std::string &segment_id,
			      const std::string &object_id,
			      int expected_size,
			      int size_given);
    ~Unexpected_size_exception() throw ();
    const char * what () const throw ();
    
  private:
    
    std::string error_message_;

  };


}
