/**
 * @file simpleId.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Simple id for serializable messages
 */
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
