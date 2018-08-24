/**
  *
  */

#include <set>
#include <string>
#include <map>
#include <memory>
#include <iostream>

#include <eigen3/Eigen/Core>
#include <boost/interprocess/managed_shared_memory.hpp>

#include "shared_memory/exceptions.h"

#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#define SHARED_MEMORY_SIZE 65536
#define MAP_STRING_KEY_SEPARATOR ';'

// cool doc: https://theboostcpplibraries.com/boost.interprocess-managed-shared-memory
//           https://www.boost.org/doc/libs/1_63_0/doc/html/interprocess/quick_guide.html


/**
 * All templated types in this namespaces are elementary types:
 * int, double, float, char*, ...
 */
namespace shared_memory {
  /**
   * @brief SHMObjects typedef is a simple renaming that ease the for loop
   * writting.
   */
  typedef  std::map<std::string, void*> SHMObjects;

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
    ~SharedMemorySegment();

    /**
     * @brief objects_ all all the data stored in the segment. WARNING here we
     * use void* so the use of the set and get functions is the RESPONSABILITY
     * of the user.
     *
     * The user is to use the SAME type when calling set and get using the
     * shared memory
     */
    SHMObjects objects_;

    /**
     * @brief shm_segment is the boost object that manages the shared memory
     * segment
     */
    boost::interprocess::managed_shared_memory shm_segment_;

  private:

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

  /**
   * @brief get_segment creates or give back a pointer to a shared memory
   * segment manager.
   * @param segment_id is the name of the shared memory segment.
   */
  const boost::interprocess::managed_shared_memory& get_segment_manager(
      const std::string &segment_id,
      const bool clear_upon_destruction=false);

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
   * @brief delete_object deletes a list of particular objects in the shared
   * memory segment. All objects must be of the same type
   * @param[in] segment_id is the name of the shared memory segment.
   * @return true if everything went fine.
   */
  template <typename ElemType>
  bool delete_object(const std::string& segment_id,
                     const std::vector<std::string>& object_id);

  /**
   * @brief get_sgement_mutex aquiere a reference to the semgent global mutex.
   * @param[in] segment_id is the name of the shared memory segment.
   * @return a reference to a boost mutex
   */
  boost::interprocess::interprocess_mutex* get_segment_mutex(
      const std::string segment_id);

  /**
   * @brief this set function instanciates or get pointer to a fixed sized
   * array of the templated type "T" in the shared memory.
   * [in] segment_id is the name of the shared memory segment.
   * [in] object_id is the name of the shared memory object to set.
   * [in] object is the pointer to the array of objects to set in the memory.
   * [in] size is the array size.
   */
  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const T* object,
           const std::size_t size);


  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::string &set_);
  
  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const T &set_);
  {
    // if T turns out to be a std::string,
    // calling specialized function
//    if (std::is_same<T, std::string>::value){
//      set(segment_id,object_id,set_);
//      return;
//    }
//    try {
//      boost::interprocess::managed_shared_memory segment{
//        boost::interprocess::open_or_create,
//        segment_id.c_str(),SHARED_MEMORY_SIZE};
//      boost::interprocess::named_mutex mutex{
//        boost::interprocess::open_or_create,object_id.c_str()};

//      mutex.lock();
//      T* object = segment.find_or_construct<T>(object_id.c_str())();
//      *object = set_;
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);
//    }

  }




  
  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::vector<T> &set_){

//    try {

//      boost::interprocess::managed_shared_memory segment{
//        boost::interprocess::open_or_create,
//        segment_id.c_str(),
//        SHARED_MEMORY_SIZE
//      };
//      boost::interprocess::named_mutex mutex{
//        boost::interprocess::open_or_create,
//        object_id.c_str()
//      };
      
//      mutex.lock();
//      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
//      for(int i=0;i<set_.size();i++) object[i]=set_[i];
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);
//    }
  }

  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::vector<T*> &set_){
    
//    try {

//      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_SIZE};
//      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
//      mutex.lock();
//      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
//      for(int i=0;i<set_.size();i++) {
//        object[i]=*(set_[i]);
//      }
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);
//    }

  }

  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const Eigen::Matrix<T, Eigen::Dynamic, 1> &set_)
  {
//    try {

//      boost::interprocess::managed_shared_memory segment{
//        boost::interprocess::open_or_create,
//        segment_id.c_str(),
//        SHARED_MEMORY_SIZE
//      };
//      boost::interprocess::named_mutex mutex{
//        boost::interprocess::open_or_create,
//        object_id.c_str()
//      };

//      mutex.lock();
//      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
//      for(int i=0;i<set_.size();i++)
//      {
//        object[i] = set_(i);
//      }
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);
//    }
  }
  
  template<typename KEY, typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           std::map<KEY,VALUE> &set_){

//    try {

//      std::string keys_object_id = std::string("key_")+object_id;
//      std::string values_object_id = std::string("values_")+object_id;
      
//      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_SIZE};
//      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
//      mutex.lock();
//      KEY* keys = segment.find_or_construct<KEY>(keys_object_id.c_str())[set_.size()]();
//      VALUE* values = segment.find_or_construct<VALUE>(values_object_id.c_str())[set_.size()]();
//      int i=0;
//      for (const auto& key_value: set_){
//        keys[i]=key_value.first;
//        values[i]=key_value.second;
//        i++;
//      }
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);
//    }

  }


  template<typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           std::map<std::string,VALUE> &set_){

//    for (const auto& key_value: set_){
//      std::string object_id_ = object_id+std::string("_")+key_value.first;
//      set(segment_id,object_id_,key_value.second);
//    }

  }


  template<typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::map<std::string,std::vector<VALUE>> &set_){

//    for (const auto& key_value: set_){
//      std::string object_id_ = object_id+std::string("_")+key_value.first;
//      set(segment_id,object_id_,key_value.second);
//    }

  }

  template<typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::map<std::string, Eigen::Matrix
           <VALUE, Eigen::Dynamic, 1>> &set_){
//    for (const auto& key_value: set_){
//      std::string object_id_ = object_id+std::string("_")+key_value.first;
//      set(segment_id,object_id_,key_value.second);
//    }
  }
  
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::string &get_);


  template<typename T>
  void get(const std::string &segment_id,
           const std::string &object_id,
           T &get_) {


    // if T turns out to be std::string,
    // calling specialized function
//    if (std::is_same<T, std::string>::value){
//      get(segment_id,object_id,get_);
//      return;
//    }
    
//    try {
      
//      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
//      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      
//      mutex.lock();
//      get_ = * ( segment.find_or_construct<T>(object_id.c_str())() );
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);
      
//    } catch (const boost::interprocess::interprocess_exception &e){
//      return;
//    }

  }
  
  
  template<typename T>
  void get(const std::string &segment_id,
           const std::string &object_id,
           T *get_,
           std::size_t expected_size){

//    try {

//      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
//      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
//      mutex.lock();
//      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
//      if(object.second != expected_size){
//        throw shared_memory::Unexpected_size_exception(segment_id,
//                                                       object_id,
//                                                       object.second,
//                                                       expected_size);
//      }
//      for(int i=0;i<object.second;i++) get_[i]=(object.first)[i];
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);

//    } catch (const boost::interprocess::interprocess_exception &e){
//      return;
//    }


  }


  template<typename T>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::vector<T> &get_) {

//    try {

//      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
//      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
//      mutex.lock();
//      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
//      if(object.second != get_.size()){
//        throw shared_memory::Unexpected_size_exception(segment_id,
//                                                       object_id,
//                                                       object.second,
//                                                       get_.size());
//      }
      
//      for(int i=0;i<object.second;i++) {
//        get_[i]=(object.first)[i];
//      }
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);

//    } catch (const boost::interprocess::interprocess_exception &e){
//      return;
//    }
  }

  template<typename T>
  void get(const std::string &segment_id,
           const std::string &object_id,
           Eigen::Matrix<T, Eigen::Dynamic, 1> &get_) {
//    try {

//      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
//      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

//      mutex.lock();
//      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
//      if(object.second != get_.size()){
//        throw shared_memory::Unexpected_size_exception(segment_id,
//                                                       object_id,
//                                                       object.second,
//                                                       get_.size());
//      }

//      for(int i=0;i<object.second;i++) {
//        get_(i)=(object.first)[i];
//      }
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);

//    } catch (const boost::interprocess::interprocess_exception &e){
//      return;
//    }
  }

  template<typename VALUE>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::map<std::string,VALUE> &get_){

//    for (const auto& key_value: get_){
//      std::string object_id_ = object_id + std::string("_")+key_value.first;
//      get(segment_id,object_id_,get_[key_value.first]);
//    }

  }

  
  template<typename KEY, typename VALUE>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::map<KEY,VALUE> &get_){

//    try {
      
//      std::string keys_object_id = std::string("key_")+object_id;
//      std::string values_object_id = std::string("values_")+object_id;
      
//      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
//      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
//      mutex.lock();
//      std::pair<KEY*, std::size_t> keys = segment.find<KEY>(keys_object_id.c_str());
//      std::pair<VALUE*, std::size_t> values = segment.find<VALUE>(values_object_id.c_str());
//      if(keys.second != get_.size()){
//        throw shared_memory::Unexpected_size_exception(segment_id,
//                                                       object_id,
//                                                       keys.second,
//                                                       get_.size());
//      }
//      for(int i=0;i<keys.second;i++){
//        get_[(keys.first)[i]]=(values.first)[i];
//      }
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);

//    } catch (const boost::interprocess::interprocess_exception &e){
//      return;
//    }


  }

}

#include<shared_memory/shared_memory.hxx>

#endif // SHARED_MEMORY_HPP
