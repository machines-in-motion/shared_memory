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
#include <boost/interprocess/sync/named_mutex.hpp>

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
   * @brief SHMObjects typdef is a simple renaming that ease the for loop
   * writting.
   */
  typedef  std::map<std::string, void*> SHMObjects;

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
    SharedMemorySegment(std::string name);

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

    std::string name_;
  };

  /**
   * @brief delete_segment deletes the segment of existing shared memory.
   * it makes sure that all element created in it is destroyed first.
   * (is this needed? I do not know.)
   * @param segment_id is the name of the shared memory segment.
   */
  void delete_segment(const std::string &segment_id);

  /**
   * @brief delete_object deletes a particular object in the shared memory
   * segment
   * @param segment_id
   */
  void delete_object(const std::string &segment_id);


  void clear_mutex(const std::string &object_id);
  void clear_mutexes(const std::vector<std::string> &mutexes);
  void clear(const std::string &segment_id,
             const std::vector<std::string> &mutexes);
  void clear(const std::string &segment_id,
             const std::vector<std::string> &mutexes,
             const std::map<std::string,std::vector<std::string>> &map_keys);


  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::string &set_);

  
  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const T &set_){
    // if T turns out to be a std::string,
    // calling specialized function
    if (std::is_same<T, std::string>::value){
      set(segment_id,object_id,set_);
      return;
    }    
    try {
      boost::interprocess::managed_shared_memory segment{
        boost::interprocess::open_or_create,
        segment_id.c_str(),SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create,object_id.c_str()};

      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())();
      *object = set_;
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

  }


  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const T *set_,
           std::size_t size){

    try {

      boost::interprocess::managed_shared_memory segment{
        boost::interprocess::open_or_create,
        segment_id.c_str(),
        SHARED_MEMORY_SIZE
      };
      boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create,
        object_id.c_str()
      };
      
      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[size]();
      for(int i=0;i<size;i++) object[i]=set_[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

  }


  
  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::vector<T> &set_){

    try {

      boost::interprocess::managed_shared_memory segment{
        boost::interprocess::open_or_create,
        segment_id.c_str(),
        SHARED_MEMORY_SIZE
      };
      boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create,
        object_id.c_str()
      };
      
      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
      for(int i=0;i<set_.size();i++) object[i]=set_[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
  }

  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::vector<T*> &set_){
    
    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
      for(int i=0;i<set_.size();i++) {
        object[i]=*(set_[i]);
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

  }

  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const Eigen::Matrix<T, Eigen::Dynamic, 1> &set_)
  {
    try {

      boost::interprocess::managed_shared_memory segment{
        boost::interprocess::open_or_create,
        segment_id.c_str(),
        SHARED_MEMORY_SIZE
      };
      boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create,
        object_id.c_str()
      };

      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[set_.size()]();
      for(int i=0;i<set_.size();i++)
      {
        object[i] = set_(i);
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
  }
  
  template<typename KEY, typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           std::map<KEY,VALUE> &set_){

    try {

      std::string keys_object_id = std::string("key_")+object_id;
      std::string values_object_id = std::string("values_")+object_id;
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_or_create,segment_id.c_str(),SHARED_MEMORY_SIZE};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      KEY* keys = segment.find_or_construct<KEY>(keys_object_id.c_str())[set_.size()]();
      VALUE* values = segment.find_or_construct<VALUE>(values_object_id.c_str())[set_.size()]();
      int i=0;
      for (const auto& key_value: set_){
        keys[i]=key_value.first;
        values[i]=key_value.second;
        i++;
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }

  }


  template<typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           std::map<std::string,VALUE> &set_){

    for (const auto& key_value: set_){
      std::string object_id_ = object_id+std::string("_")+key_value.first;
      set(segment_id,object_id_,key_value.second);
    }

  }


  template<typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::map<std::string,std::vector<VALUE>> &set_){

    for (const auto& key_value: set_){
      std::string object_id_ = object_id+std::string("_")+key_value.first;
      set(segment_id,object_id_,key_value.second);
    }

  }

  template<typename VALUE>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::map<std::string, Eigen::Matrix
           <VALUE, Eigen::Dynamic, 1>> &set_){
    for (const auto& key_value: set_){
      std::string object_id_ = object_id+std::string("_")+key_value.first;
      set(segment_id,object_id_,key_value.second);
    }
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
    if (std::is_same<T, std::string>::value){
      get(segment_id,object_id,get_);
      return;
    }
    
    try {
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      
      mutex.lock();
      get_ = * ( segment.find_or_construct<T>(object_id.c_str())() );
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);
      
    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }

  }
  
  
  template<typename T>
  void get(const std::string &segment_id,
           const std::string &object_id,
           T *get_,
           std::size_t expected_size){

    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
      if(object.second != expected_size){
        throw shared_memory::Unexpected_size_exception(segment_id,
                                                       object_id,
                                                       object.second,
                                                       expected_size);
      }
      for(int i=0;i<object.second;i++) get_[i]=(object.first)[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }


  }


  template<typename T>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::vector<T> &get_) {

    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
      if(object.second != get_.size()){
        throw shared_memory::Unexpected_size_exception(segment_id,
                                                       object_id,
                                                       object.second,
                                                       get_.size());
      }
      
      for(int i=0;i<object.second;i++) {
        get_[i]=(object.first)[i];
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }
  }

  template<typename T>
  void get(const std::string &segment_id,
           const std::string &object_id,
           Eigen::Matrix<T, Eigen::Dynamic, 1> &get_) {
    try {

      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};

      mutex.lock();
      std::pair<T*, std::size_t> object = segment.find<T>(object_id.c_str());
      if(object.second != get_.size()){
        throw shared_memory::Unexpected_size_exception(segment_id,
                                                       object_id,
                                                       object.second,
                                                       get_.size());
      }

      for(int i=0;i<object.second;i++) {
        get_(i)=(object.first)[i];
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }
  }

  template<typename VALUE>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::map<std::string,VALUE> &get_){

    for (const auto& key_value: get_){
      std::string object_id_ = object_id + std::string("_")+key_value.first;
      get(segment_id,object_id_,get_[key_value.first]);
    }

  }

  
  template<typename KEY, typename VALUE>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::map<KEY,VALUE> &get_){

    try {
      
      std::string keys_object_id = std::string("key_")+object_id;
      std::string values_object_id = std::string("values_")+object_id;
      
      boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only,segment_id.c_str()};
      boost::interprocess::named_mutex mutex{boost::interprocess::open_or_create, object_id.c_str()};
      
      mutex.lock();
      std::pair<KEY*, std::size_t> keys = segment.find<KEY>(keys_object_id.c_str());
      std::pair<VALUE*, std::size_t> values = segment.find<VALUE>(values_object_id.c_str());
      if(keys.second != get_.size()){
        throw shared_memory::Unexpected_size_exception(segment_id,
                                                       object_id,
                                                       keys.second,
                                                       get_.size());
      }
      for(int i=0;i<keys.second;i++){
        get_[(keys.first)[i]]=(values.first)[i];
      }
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }


  }

}

#include<shared_memory/shared_memory.hxx>

#endif // SHARED_MEMORY_HPP
