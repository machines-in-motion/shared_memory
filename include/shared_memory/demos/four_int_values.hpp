#ifndef FOUR_INT_VALUES_HPP
#define FOUR_INT_VALUES_HPP

#include "shared_memory/serializable.hpp"
#include <iostream>

namespace shared_memory {


  class Four_int_values : public Serializable<5,0> {

  public:
    
    Four_int_values();
    Four_int_values(int a, int b, int c, int d);
    void create(double *serialized_data);
    void serialize(double *serialized) const;
    int get_id() const;
    
  public:

    // stored values are all equals, in same order
    bool equal(const Four_int_values &other) const;

    // equal, + same id
    bool same(const Four_int_values &other) const;

  public:

    void print() const;
    
    
  private:

    static int next_id();
    
  private:

    int values_[4];
    int id_;
    
  };

}

#endif
