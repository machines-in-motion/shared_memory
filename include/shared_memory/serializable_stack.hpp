#ifndef SERIALIZABLE_STACK_HPP
#define SERIALIZABLE_STACK_HPP

#include "shared_memory/serializable.hpp"
#include <deque>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace shared_memory {

  template <class Serializable>
  class Serializable_stack {

  public:

    Serializable_stack(int max_items);
    ~Serializable_stack();

    bool add(const Serializable &serializable);

    void remove(int nb_items, std::deque<int> &get_removed_ids);
    void remove(int nb_items);
    void remove(int nb_items, std::deque<int> *get_removed_ids);

  public:

    double * const get_data();
    int get_data_size();
    
  private:

    bool external_;
    double *data_;
    int array_size_;
    int items_serialization_size_;
    int nb_items_;
    int items_max_numbers_;
    int id_;
    
  };

  template <class Serializable>
  class Serializable_stack_reader {

  public:

    Serializable_stack_reader(int max_items);
    ~Serializable_stack_reader();
    void reset(double *data);
    double * const get_data_and_reset();
    double * const get_data();
    int get_index();
    void reset();
    void reset(int nb_removed);
    bool empty();
    void read(Serializable &get);
    int get_id();

  public:

    int get_data_size();
    
  private:
    int array_size_;
    int index_;
    double *data_;

  };
  
  

  #include "serializable_stack.hxx"

}


#endif
