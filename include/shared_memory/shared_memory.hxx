#pragma once

/**
 * \file shared_memory.hpp
 * \brief Shared memory encapsulation
 * \author Vincent Berenz
 * \author Maximilien Naveau
 * \date 2018
 *
 * This file implements the functions from shared_memory.hpp that encapsulate
 * the use of the shared memory using the boost::interprocess package.
 * usage: see demos and unit tests and documentation
 */

#include <shared_memory/shared_memory.hpp>

namespace shared_memory {

  /***********************************************
   * Definition of the SharedMemorySegment class *
   ***********************************************/
  template<typename ElemType>
  void SharedMemorySegment::get_object(const std::string& object_id,
                                       std::pair<ElemType*, std::size_t>& get_)
  {

    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*mutex_);

    //register_object(object_id, get_);

    bool registered = register_object_read_only<ElemType>(object_id);
    if(objects_[object_id].second != get_.second){
      delete_object<ElemType>(object_id);
      throw shared_memory::Unexpected_size_exception(segment_id_,
						     object_id,
						     objects_[object_id].second,
						     get_.second);
    }

    if (registered){
      std::cout << "registration of " << object_id
		<< " of size " << get_.second << std:: endl;
    }
    
    ElemType* shared_data = static_cast<ElemType*>(objects_[object_id].first);
    std::size_t shared_data_size = objects_[object_id].second;
    for(std::size_t i = 0 ; i < shared_data_size ; ++i)
      {
	get_.first[i] = shared_data[i];
      }

  }

  template<typename ElemType>
  void SharedMemorySegment::set_object(const std::string& object_id,
      const std::pair<const ElemType*, std::size_t>& set_)
  {

    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*mutex_);

    bool registered = register_object(object_id, set_);

    if (registered){
      std::cout << "registration of " << object_id
		<< " of size " << objects_[object_id].second << std:: endl;
    }

    ElemType* shared_data = static_cast<ElemType*>(objects_[object_id].first);
    std::size_t shared_data_size = objects_[object_id].second;
    for(std::size_t i = 0 ; i < shared_data_size ; ++i)
    {
      shared_data[i] = set_.first[i];
    }

  }

  template<typename ElemType>
  bool SharedMemorySegment::register_object(
      const std::string& object_id,
      const std::pair<ElemType*, std::size_t>& obj_)
  {
    if(is_object_registered(object_id))
    {
      return false;
    }
    std::size_t obj_size = 0;
    typename std::remove_const<ElemType>::type * obj_ptr = nullptr;

    obj_ptr = segment_manager_.find_or_construct<
              typename std::remove_const<ElemType>::type>(
                object_id.c_str())[obj_.second]();
    obj_size = obj_.second;

    objects_[object_id] = std::pair<void*, std::size_t>();
    objects_[object_id].first = static_cast<void*>(obj_ptr);
    objects_[object_id].second = obj_size;

    return true;
    std::cout << "registration of " << object_id
              << " of size " << obj_size << std:: endl;
  }

  template<typename ElemType>
  bool SharedMemorySegment::register_object_read_only(
      const std::string& object_id)
  {
    if(is_object_registered(object_id))
    {
      return false;
    }

    std::size_t obj_size = 0;
    typename std::remove_const<ElemType>::type * obj_ptr = nullptr;

    std::pair<typename std::remove_const<ElemType>::type*, std::size_t>
        obj_pair = segment_manager_.find<
                   typename std::remove_const<ElemType>::type>(
                     object_id.c_str());
    obj_ptr = obj_pair.first;
    obj_size = obj_pair.second;

    objects_[object_id] = std::pair<void*, std::size_t>();
    objects_[object_id].first = static_cast<void*>(obj_ptr);
    objects_[object_id].second = obj_size;

    return true;
    
    std::cout << "registration of " << object_id
              << " of size " << obj_size << std:: endl;
  }

  template<typename ElemType>
  void SharedMemorySegment::delete_object(const std::string& object_id)
  {
    try {
      if(objects_.count(object_id) != 0)
      {
        objects_.erase(object_id);
      }
      segment_manager_.destroy<ElemType>(object_id.c_str());
    } catch (const boost::interprocess::interprocess_exception&){
      return;
    }
  }

  /*************************************************
   * Definition of all segment management function *
   *************************************************/

  /**
   * @brief SegmentMap typedef is a simple short cut to the GLOBAL_SHM_SEGMENTS
   * type.
   */
  typedef std::map<std::string, std::unique_ptr<SharedMemorySegment> >
  SegmentMap;

  /**
   * @brief GLOBAL_SHARED_MEMORY_SEGMENT is global variable that acts as a
   * a shared memory manager.
   *
   * The use of the std::unique_ptr allows to delete the object and re-create
   * at will.
   */
  static SegmentMap GLOBAL_SHM_SEGMENTS;

  template<typename ElemType>
  bool delete_object(const std::string& segment_id,
                     const std::string& object_id){
    try {
      SharedMemorySegment& segment = get_segment(segment_id);
      segment.delete_object<ElemType>(object_id);
    } catch (const boost::interprocess::interprocess_exception&){
      return false;
    }
  }

  /***********************************
   * Definition of all set functions *
   ***********************************/

  template<typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const ElemType& set_)
  {
    try {
      SharedMemorySegment& segment = get_segment(segment_id);
      std::pair<const ElemType*, std::size_t> set_ref;
      set_ref.first = &set_;
      set_ref.second = 1 ;
      segment.set_object<ElemType>(object_id, set_ref);
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
  }

  template<typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const ElemType* set_,
           const std::size_t size)
  {
    try {
      SharedMemorySegment& segment = get_segment(segment_id);
      std::pair<const ElemType*, std::size_t> set_ref;
      set_ref.first = set_;
      set_ref.second = size ;
      segment.set_object<ElemType>(object_id, set_ref);
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
  }

  template<typename VectorType, typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const VectorType& set_)
  {
    set<ElemType>(segment_id, object_id, set_.data(), set_.size());
  }

  template<typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::vector<ElemType>& set_)
  {
    set<ElemType>(segment_id, object_id, set_.data(), set_.size());
  }

  template<typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const Eigen::Matrix<ElemType, Eigen::Dynamic, 1>& set_)
  {
    set<ElemType>(segment_id, object_id, set_.data(), set_.size());
  }

  template<typename FirstType, typename SecondType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::pair<FirstType, SecondType>& set_)
  {
    set<FirstType>(segment_id, object_id + "_first", set_.first);
    set<SecondType>(segment_id, object_id + "_second", set_.second);
  }

  template<typename KeyType, typename ValueType>
  void set(const std::string& segment_id,
           const std::string& object_id,
           const std::map<KeyType, ValueType>& set_)
  {
    try {
      int i = 0;
      for(typename std::map<KeyType, ValueType>::const_iterator
          map_it=set_.begin() ; map_it!=set_.end() ; ++map_it)
      {
        set(segment_id,
            object_id + "_" + std::to_string(i),
            map_it->second);
        ++i;
      }
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
  }

  /***********************************
   * Definition of all get functions *
   ***********************************/

  template<typename ElemType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           ElemType& get_)
  {
    try {
      SharedMemorySegment& segment = get_segment(segment_id);
      std::pair<ElemType*, std::size_t> get_ref;
      get_ref.first = &get_;
      get_ref.second = 1 ;
      segment.get_object<ElemType>(object_id, get_ref);
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }catch (const boost::interprocess::interprocess_exception&){
      return;
    }
  }

  template<typename ElemType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           ElemType* get_,
           const std::size_t expected_size)
  {
    try {
      SharedMemorySegment& segment = get_segment(segment_id);
      std::pair<ElemType*, std::size_t> get_ref;
      get_ref.first = get_;
      get_ref.second = expected_size ;
      segment.get_object<ElemType>(object_id, get_ref);
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }catch (const boost::interprocess::interprocess_exception&){
      return;
    }
  }

  template<typename ElemType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::vector<ElemType>& get_)
  {
    get<ElemType>(segment_id, object_id, &(get_[0]), get_.size());
  }

  template<typename ElemType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           Eigen::Matrix<ElemType, Eigen::Dynamic, 1>& get_)
  {
    get(segment_id, object_id, get_.data(), get_.size());
  }

  template<typename FirstType, typename SecondType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           std::pair<FirstType, SecondType>& get_)
  {
    get<FirstType>(segment_id, object_id + "_first", get_.first);
    get<SecondType>(segment_id, object_id + "_second", get_.second);
  }

  template<typename KeyType, typename ValueType>
  void get(const std::string& segment_id,
           const std::string& object_id,
           std::map<KeyType, ValueType>& get_)
  {
    try {
      int i = 0;
      for(typename std::map<KeyType, ValueType>::iterator
          map_it=get_.begin() ; map_it!=get_.end() ; ++map_it)
      {
        get(segment_id,
            object_id + "_" + std::to_string(i),
            map_it->second);
        ++i;
      }
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
  }

} // namespace shared_memory
