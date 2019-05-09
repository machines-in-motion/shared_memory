#ifndef SERIALIZABLE_EXCHANGE_HPP
#define SERIALIZABLE_EXCHANGE_HPP

#include <string>
#include <deque>
#include <stdexcept>
#include <cstring>	

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializable.hpp"

namespace shared_memory {

  template<class Serializable>
  class Serializable_exchange {
    
  public:
    
    Serializable_exchange(std::string segment_id,
			  std::string object_id);

    ~Serializable_exchange();

    void set(const Serializable &serializable);
    void read(Serializable &serializable);

  private:

    std::string segment_id_;
    std::string object_id_;
    double *data_;

  };

  
  #include "serializable_exchange.hxx"

}

#endif 
