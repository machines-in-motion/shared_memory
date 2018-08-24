#include <shared_memory/shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

namespace shared_memory {

  /***********************************************
   * Definition of the SharedMemorySegment class *
   ***********************************************/
  SharedMemorySegment::SharedMemorySegment(std::string segment_id,
                                           bool clear_upon_destruction)
  {
    // save the id the of the segment
    segment_id_ = segment_id;

    // check if we should delete the memory upon destruction.
    clear_upon_destruction_ = clear_upon_destruction;

    // create and/or map the memory segment
    segment_manager_ = boost::interprocess::managed_shared_memory(
                    boost::interprocess::open_or_create,
                    segment_id.c_str(),
                    SHARED_MEMORY_SIZE);

  }

  SharedMemorySegment::~SharedMemorySegment()
  {
    if(clear_upon_destruction_)
    {
      boost::interprocess::shared_memory_object::remove(segment_id_.c_str());
    }
  }

  template<typename ElemType>
  void SharedMemorySegment::get_object(const std::string& object_id,
                                       std::pair<ElemType*, std::size_t>& get_)
  {
    if(!is_object_registered(object_id))
    {
      std::pair<ElemType*, std::size_t> shared_object;
      shared_object = segment_manager_.find<ElemType>(object_id.c_str());
      if(shared_object.second != get_.second)
      {
        throw shared_memory::Unexpected_size_exception(
              segment_id_, object_id, static_cast<int>(shared_object.second),
              static_cast<int>(get_.second));
      }
      objects_[object_id] = std::pair<void*, std::size_t>();
      objects_[object_id].first = static_cast<void*>(shared_object.first);
      objects_[object_id].second = shared_object.second;
    }
    boost::interprocess::named_mutex named_mtx{
      boost::interprocess::open_or_create, "mtx"
    };
    named_mtx.lock();
    ElemType* shared_data = static_cast<ElemType*>(objects_[object_id].first);
    for(std::size_t i = 0 ; i < objects_[object_id].second ; ++i)
    {
      get_.first[i] = shared_data[i];
    }
    named_mtx.unlock();
    destroy_mutex();
  }

  template<typename ElemType>
  void SharedMemorySegment::set_object(const std::string& object_id,
      const std::pair<const ElemType*, std::size_t>& set_)
  {
    if(!is_object_registered(object_id))
    {
      ElemType* shared_object = segment_manager_.find_or_construct<ElemType>(
                                  object_id.c_str())[set_.second]();
      objects_[object_id] = std::pair<void*, std::size_t>();
      objects_[object_id].first = static_cast<void*>(shared_object);
      objects_[object_id].second = set_.second;
    }
    boost::interprocess::named_mutex named_mtx{
      boost::interprocess::open_or_create, "mtx"
    };
    named_mtx.lock();
    ElemType* shared_data = static_cast<ElemType*>(objects_[object_id].first);
    for(std::size_t i = 0 ; i < objects_[object_id].second ; ++i)
    {
      shared_data[i] = set_.first[i];
    }
    named_mtx.unlock();
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

  SharedMemorySegment& get_segment(
      const std::string &segment_id,
      const bool clear_upon_destruction)
  {
    if(GLOBAL_SHM_SEGMENTS.count(segment_id) == 0)
    {
      GLOBAL_SHM_SEGMENTS[segment_id].reset(
            new SharedMemorySegment(segment_id, clear_upon_destruction));
    }
    GLOBAL_SHM_SEGMENTS[segment_id]->set_clear_upon_destruction(
          clear_upon_destruction);
    return *GLOBAL_SHM_SEGMENTS[segment_id];
  }


  boost::interprocess::interprocess_mutex& get_segment_mutex(
      const std::string segment_id)
  {
    SharedMemorySegment& segment = get_segment(segment_id);
    segment.create_mutex();
    return *segment.mutex_;
  }

  void delete_segment(const std::string &segment_id){
    get_segment(segment_id, true);
    // here the unique pointer destroy the object for us.
    GLOBAL_SHM_SEGMENTS.erase(segment_id);
  }

  void delete_all_segment(){
    for(SegmentMap::iterator seg_it=GLOBAL_SHM_SEGMENTS.begin() ;
        seg_it!=GLOBAL_SHM_SEGMENTS.end() ; seg_it=GLOBAL_SHM_SEGMENTS.begin())
    {
      get_segment(seg_it->second->get_segment_id(), true);
      GLOBAL_SHM_SEGMENTS.erase(seg_it);
    }
  }

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

  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::string &set_){
    set<char>(segment_id,
              object_id,
              set_.c_str(),
              set_.size());
  }

  template<typename VectorType, typename ElemType>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const VectorType& set_)
  {
    set<ElemType>(segment_id, object_id, set_.data(), set_.size());
  }

  template<typename KeyType, typename ValueType>
  void set(const std::string& segment_id,
           const std::string& object_id,
           const std::map<KeyType, ValueType>& set_)
  {
    try {
      SharedMemorySegment& segment = get_segment(segment_id);

      std::string keys_object_id = object_id + "_keys";
      std::pair<KeyType*, std::size_t> keys;
      segment.get_object<KeyType>(object_id, keys);

      std::string values_object_id = object_id + "_values";
      std::pair<ValueType*, std::size_t> values;
      segment.get_object<ValueType>(object_id, values);

      segment.mutex_->lock();
      int i = 0;
      for(typename std::map<KeyType, ValueType>::const_iterator
          map_it = set_.begin() ; map_it != set_.end() ; ++map_it)
      {
        keys.first[i] = map_it->first;
        values.first[i] = map_it->second;
        ++i;
      }
      segment.mutex_->unlock();
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

  void get(const std::string &segment_id,
           const std::string &object_id,
           std::string &get_){
    char* new_string = new char[get_.size()];
    std::size_t size = get_.size();
    get_.clear();
    get<char>(segment_id, object_id, new_string, size);
    get_= std::string(new_string, size);
  }

  template<typename VectorType, typename ElemType>
  void get(const std::string &segment_id,
           const std::string &object_id,
           VectorType& get_)
  {
    get<ElemType>(segment_id, object_id, get_.data(), get_.size());
  }

  template<typename KeyType, typename ValueType>
  void get(const std::string& segment_id,
           const std::string& object_id,
           std::map<KeyType, ValueType>& get_)
  {
    try {
      SharedMemorySegment& segment = get_segment(segment_id);

      std::string keys_object_id = object_id + "_keys";
      std::pair<KeyType*, std::size_t> keys;
      segment.get_object<KeyType>(object_id, keys);

      std::string values_object_id = object_id + "_values";
      std::pair<ValueType*, std::size_t> values;
      segment.get_object<ValueType>(object_id, values);

      segment.mutex_->lock();
      for(int i=0;i<keys.second;i++){
        if(get_.count((keys.first)[i]) == 0)
        {
          throw Unexpected_map_key<KeyType>(
                segment_id, object_id, (keys.first)[i]);
        }
        get_[(keys.first)[i]]=(values.first)[i];
      }
      segment.mutex_->unlock();
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }
  }

} // namespace shared_memory
