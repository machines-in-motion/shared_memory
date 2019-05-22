/**
 * @file serializable.hpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Defines an abstract interface to serializable objects.
 */

#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP


namespace shared_memory {

  template <int SERIALIZATION_SIZE, int SERIALIZED_ID_INDEX>
  class Serializable {

  public:

    virtual void create(double *serialized_data) = 0;
    virtual void serialize(double *serialized) const = 0;
    virtual int get_id() const = 0;
    
  public:

    static const int serialization_size=SERIALIZATION_SIZE;
    static const int serialization_id_index=SERIALIZED_ID_INDEX;
    
  };


}


#endif
