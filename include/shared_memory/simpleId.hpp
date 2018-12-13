#ifndef SIMPLE_ID_HPP
#define SIMPLE_ID_HPP

#include "shared_memory/serializable.hpp"


namespace shared_memory {


  class SimpleId : public Serializable<1,0> {

  public:

    SimpleId();
    SimpleId(int id);
    void create(double *serialized_data);
    void serialize(double *serialized) const;
    int get_id() const;
    
    
  private:

    int id_;
    
  };

}

#endif
