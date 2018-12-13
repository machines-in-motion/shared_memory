/**
 * \file shared_memory.hpp
 * \brief Shared memory encapsulation
 * \author Vincent Berenz
 * \author Maximilien Naveau
 * \date 2018
 *
 * This file declares some function that encapsulate the use of the shared
 * memory using the boost::interprocess package.
 * usage: see demos and unit tests and documentation
 */

#pragma once
#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <eigen3/Eigen/Core>

#include <boost/interprocess/managed_shared_memory.hpp>

#include "shared_memory/exceptions.h"

#define SHARED_MEMORY_SIZE 65536
#define MAP_STRING_KEY_SEPARATOR ';'

// cool doc:
// https://theboostcpplibraries.com/boost.interprocess-managed-shared-memory
// https://www.boost.org/doc/libs/1_63_0/doc/html/interprocess/quick_guide.html


/**
 * All templated types in this namespaces are elementary types:
 * int, double, float, char*, ...
 */
namespace shared_memory {
  /***********************
   * Typdef declarations *
   ***********************/

  /**
   * @brief ShmObjects typedef is a simple renaming that ease the for loop
   * writting.
   */
  typedef  std::map<std::string, std::pair<void*, std::size_t>> ShmObjects;

  /**
   * @brief ShmTypeHelper is a small struct that allow the definition of
   * templated typedef.
   */
  template<typename ElemType>
  struct ShmTypeHelper{
    /**
     * @brief ShmemAllocator typedef allows to create std::allocator with the
     * boost interprocess library.
     */
    typedef boost::interprocess::allocator
    <ElemType, boost::interprocess::managed_shared_memory::segment_manager>
    ElemTypeAllocator;

    /**
     * @brief Vector typedef allows to instanciate std::vector inside the shared
     * memory
     */
    typedef std::vector<ElemType, ElemTypeAllocator> Vector;
  };

  /************************************************
   * Declaration of the SharedMemorySegment class *
   ************************************************/

  /**
   * @brief The SharedMemorySegment contains the pointers of the shared objects
   * in on shared memrory segment
   *
   * We use unamed mutext (interprocess_mutex) and unamed condition variables
   * (interprocess_condition) to be able to instanciate them with classic
   * pointers
   */
  class SharedMemorySegment{
  public:
    /**
     * @brief SharedMemorySegment constructor.
     */
    SharedMemorySegment(std::string segment_id,
                        bool clear_upon_destruction);

    /**
      * @brief SharedMemorySegment destructor.
      */
    ~SharedMemorySegment(){}

    /**
     * @brief clear_memory free the shared memory
     */
    void clear_memory();

    /**
     * @brief get_object registers the object in the current struc and in the
     * shared memory once only. And returns the pointer to the object and its
     * size. The size will be 1 for simple type and could greater to one for
     * arrays.
     * @param[in] object_id: the name of the object in the shared memory.
     * @param[in][out] get_: the reference to the fetched object.
     */
    template<typename ElemType>
    void get_object(const std::string& object_id,
                    std::pair<ElemType*, std::size_t>& get_);

    /**
     * @brief get_object registers the object in the current struc and in the
     * shared memory once only. And returns the pointer to the object and its
     * size. The size will be 1 for simple type and could greater to one for
     * arrays.
     * @param[in] object_id: the name of the object in the shared memory.
     * @param[in][out] get_: the reference to the fetched object.
     */
    void get_object(const std::string& object_id,
                    std::string& get_);

    /**
     * @brief set_object registers the object in the current struc and in the
     * shared memory once only. And returns the pointer to the object and its
     * size. The size will be 1 for simple type and could greater to one for
     * arrays.
     * @param[in] object_id: the name of the object in the shared memory.
     * @param[in] set_: the reference to the fetched object.
     */
    template<typename ElemType>
    void set_object(const std::string& object_id,
                    const std::pair<const ElemType*, std::size_t>& set_);

    /**
     * @brief register_object registers the object in the segment uniquely.
     * @param object_id is the name of the object to register.
     * @param obj_ is the object to be registered.
     * @return true of a new object has been registered
     */
    template<typename ElemType>
    bool register_object(const std::string& object_id,
                         const std::pair<ElemType*, std::size_t>& obj_);

    /**
     * @brief register_object_read_only registers the object in the segment
     * uniquely.
     * @param object_id is the name of the object to register
     * @param obj_ is the object to be registered
     * @return true of a new object has been registered
     */
    template<typename ElemType>
    bool register_object_read_only(const std::string& object_id);

    /**
     * @brief delete_object delete and object from the shared memory.
     * @param[in] object_id: the name of the object in the shared memory.
     */
    template<typename ElemType>
    void delete_object(const std::string& object_id);

    /**
     * @brief mutex_ this mutex secure ALL the shared memory.
     */
    boost::interprocess::interprocess_mutex* mutex_;
    //boost::interprocess::named_mutex named_mtx_;

    /**
     * @brief create_mutex small factory that allow to make sure that the mutex
     * is created.
     */
    void create_mutex()
    {
      mutex_ = segment_manager_.find_or_construct
               <boost::interprocess::interprocess_mutex> ("mutex_") ();
    }

    /**
     * @brief destroy_mutex small destructor of the mutext to make sure that it
     * is unlock at critical time.
     */
    void destroy_mutex()
    {
      segment_manager_.destroy
          <boost::interprocess::interprocess_mutex> ("mutex_");
      mutex_ = nullptr ;
    }

    /**
     * @brief is_object_registered used to check if the object has been registered or not.
     * @param[in] object_id: the name of the object in the shared memory.
     * @return true if it has been registered
     */
    bool is_object_registered(const std::string& object_id)
    {
      return !(objects_.count(object_id) == 0 ||
               objects_[object_id].first == nullptr);
    }

    /**
     * @brief set_clear_upon_destruction is a standard setter
     * @param[in] clear_upon_destruction is the value to set
     */
    void set_clear_upon_destruction(const bool clear_upon_destruction)
    {
      clear_upon_destruction_ = clear_upon_destruction;
    }

    /**
     * @brief get_segment_id is a standard getter
     * @return the segment name
     */
    const std::string& get_segment_id()
    {
      return segment_id_;
    }

  private:

    /**
     * @brief shm_segment is the boost object that manages the shared memory
     * segment
     */
    boost::interprocess::managed_shared_memory segment_manager_;

    /**
     * @brief objects_ are all the data stored in the segment. WARNING here we
     * use void* so the use of the set and get functions is the RESPONSABILITY
     * of the user.
     *
     * The user is to use the SAME type when calling set and get using the
     * shared memory
     */
    ShmObjects objects_;

    /**
     * @brief segment_id_ is the name of the segment inside the shared memory
     */
    std::string segment_id_;

    /**
     * @brief clear_upon_destruction_ flag decides if the segment should be
     * cleared upon destruction.
     *
     * Usage: typically only one process should set this flag to true.
     */
    bool clear_upon_destruction_;
  };

  /**************************************************
   * Declaration of all segment management function *
   **************************************************/

  /**
   * @brief get_segment creates or give back a pointer to a SharedMemorySegment
   * object.
   * @param segment_id is the name of the shared memory segment.
   */
  SharedMemorySegment& get_segment(
      const std::string &segment_id,
      const bool clear_upon_destruction=false);

  /**
   * @brief delete_segment deletes the segment of existing shared memory.
   * it makes sure that all element created in it is destroyed first.
   * (is this needed? I do not know.)
   * @param segment_id is the name of the shared memory segment.
   */
  void delete_segment(const std::string &segment_id);

  /**
   * @brief delete_all_segment delete all mapping to the shared memory used
   * during the current process
   */
  void delete_all_segment();

  /**
   * @brief delete_object deletes a particular object in the shared memory
   * segment
   * @param[in] segment_id is the name of the shared memory segment.
   * @return true if everything went fine.
   */
  template<typename ElemType>
  bool delete_object(const std::string &segment_id,
                     const std::string &object_id);

  /**
   * @brief get_sgement_mutex aquiere a reference to the semgent global mutex.
   * @param[in] segment_id is the name of the shared memory segment.
   * @return a reference to a boost mutex
   */
  boost::interprocess::interprocess_mutex& get_segment_mutex(
      const std::string segment_id);

  /**
   * @brief clear_shared_memory_segment destroys the shared memory
   * @param[in] segment_id is the name of the shared memory segment.
   */
  void clear_shared_memory(const std::string& segment_id);

  /************************************
   * Declaration of all set functions *
   ************************************/

  /**
   * @brief set instanciates or get pointer to any elementary types in the
   * shared memory.
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  template<typename ElemType>
  void set(const std::string& segment_id,
           const std::string& object_id,
           const ElemType& set_);

  /**
   * @brief set instanciates or get pointer to a fixed sized
   * array of the templated type "T" in the shared memory.
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the pointer to the array of objects to set in the
   * memory.
   * @param[in] size is the array size.
   */
  template<typename ElemType>
  void set(const std::string& segment_id,
           const std::string& object_id,
           const ElemType* set_,
           const std::size_t size);

  /**
   * @brief set instanciates or get pointer to a string in the shared memory.
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  void set(const std::string& segment_id,
           const std::string& object_id,
           const std::string& set_);
  
  /**
   * @brief set instanciates or get pointer to a std::vector<ElemType>
   * in the shared memory.
   * This will translated as a fixed sized array in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  template<typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::vector<ElemType>& set_);

  /**
   * @brief set instanciates or get pointer to a
   * Eigen::Matrix<ElemType, Eigen::Dynamic, 1> in the shared memory.
   * This will translated as a fixed sized array in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  template<typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const Eigen::Matrix<ElemType, Eigen::Dynamic, 1>& set_);

  /**
   * @brief set instanciates or get pointer to a
   * std::pair<FirstType, SecondType> in the shared memory.
   * This is very usefull to dump maps in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  template<typename FirstType, typename SecondType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::pair<FirstType, SecondType>& set_);

  /**
   * @brief set instanciates or get pointer to a std::vector<ElemType> or an
   * Eigen::Matrix<ElemType, any, any> in the shared memory.
   * This will translated as a fixed sized array in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  template<typename KeyType, typename ValueType>
  void set(const std::string& segment_id,
           const std::string& object_id,
           const std::map<KeyType, ValueType>& set_);

  /************************************
   * Declaration of all get functions *
   ************************************/

  /**
   * @brief get gets a pointer to any elementary types in the
   * shared memory.
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] get_ is the string to be created in the shared memory
   */
  template<typename ElemType>
  void get(const std::string& segment_id,
           const std::string& object_id,
           ElemType& get_);

  /**
   * @brief get gets a pointer to a fixed sized
   * array of the templated type "T" in the shared memory.
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] get_ is the pointer to the array of objects to set in the
   * memory.
   * @param[in] size is the array size.
   */
  template<typename ElemType>
  void get(const std::string& segment_id,
           const std::string& object_id,
           ElemType* get_,
           const std::size_t expected_size);

  /**
   * @brief get gets a pointer to a string in the shared memory.
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] get_ is the string to be created in the shared memory
   */
  void get(const std::string& segment_id,
           const std::string& object_id,
           std::string& get_);

  /**
   * @brief get gets a pointer to a std::vector<ElemType>
   * in the shared memory.
   * This will translated as a fixed sized array in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  template<typename ElemType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::vector<ElemType>& get_);

  /**
   * @brief get gets a pointer to a
   * Eigen::Matrix<ElemType, Eigen::Dynamic, 1> in the shared memory.
   * This will translated as a fixed sized array in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] set_ is the string to be created in the shared memory
   */
  template<typename ElemType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           Eigen::Matrix<ElemType, Eigen::Dynamic, 1>& get_);

  /**
   * @brief get instanciates or get pointer to a
   * std::pair<FirstType, SecondType> in the shared memory.
   * This is very usefull to dump maps in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] get_ is the string to be created in the shared memory
   */
  template<typename FirstType, typename SecondType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::pair<FirstType, SecondType>& get_);

  /**
   * @brief get gets a pointer to a std::vector<ElemType> or an
   * Eigen::Matrix<ElemType, any, any> in the shared memory.
   * This will translated as a fixed sized array in the shared memory
   *
   * All set functions make sure that the pointer is uniquely created to avoid
   * useless computation time consumption.
   *
   * @param[in] segment_id is the name of the shared memory segment.
   * @param[in] object_id is the name of the shared memory object to set.
   * @param[in] get_ is the string to be created in the shared memory
   */
  template<typename KeyType, typename ValueType>
  void get(const std::string& segment_id,
           const std::string& object_id,
           std::map<KeyType, ValueType>& get_);

} // namespace shared_memory

#include<shared_memory/shared_memory.hxx>

#endif // SHARED_MEMORY_HPP
