#include <shared_memory/shared_memory.hpp>

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
    shm_segment_ = boost::interprocess::managed_shared_memory(
                    boost::interprocess::open_or_create,
                    segment_id.c_str(),
                    SHARED_MEMORY_SIZE);

    objects_["segment_mutex"] =
        static_cast<void*>(shm_segment_.find_or_construct<
        boost::interprocess::interprocess_mutex>("segment_mutex")());
  }

  SharedMemorySegment::~SharedMemorySegment()
  {
    if(clear_upon_destruction_)
    {
      boost::interprocess::shared_memory_object::remove(segment_id_.c_str());
    }
  }


  /**************************************
   * Definition of all global functions *
   **************************************/

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

  const boost::interprocess::managed_shared_memory& get_segment_manager(
      const std::string &segment_id,
      const bool clear_upon_destruction)
  {
    if(GLOBAL_SHM_SEGMENTS.count(segment_id) == 0)
    {
      GLOBAL_SHM_SEGMENTS[segment_id].reset(
            new SharedMemorySegment(segment_id, clear_upon_destruction));
    }
    return GLOBAL_SHM_SEGMENTS[segment_id]->shm_segment_;
  }

  SharedMemorySegment& get_segment(
      const std::string &segment_id,
      const bool clear_upon_destruction)
  {
    if(GLOBAL_SHM_SEGMENTS.count(segment_id) == 0)
    {
      GLOBAL_SHM_SEGMENTS[segment_id].reset(
            new SharedMemorySegment(segment_id, clear_upon_destruction));
    }
    return *GLOBAL_SHM_SEGMENTS[segment_id];
  }

  void delete_segment(const std::string &segment_id){
    if(GLOBAL_SHM_SEGMENTS.count(segment_id) == 0)
    {
      // here the unique pointer destroy the object for us.
      GLOBAL_SHM_SEGMENTS.erase(segment_id);
    }
  }

  void delete_all_segment(){
    // we progressively delete the first map element unitl there are no more
    for(SegmentMap::iterator seg_it=GLOBAL_SHM_SEGMENTS.begin();
        seg_it!=GLOBAL_SHM_SEGMENTS.end() ; seg_it=GLOBAL_SHM_SEGMENTS.begin())
    {
      GLOBAL_SHM_SEGMENTS.erase(seg_it);
    }
  }

  template<typename ElemType>
  bool delete_object(const std::string& segment_id,
                     const std::string& object_id){
    try {
      return get_segment_manager(segment_id).
          destroy<ElemType>(object_id.c_str());
    } catch (const boost::interprocess::interprocess_exception&){
      return false;
    }
  }

  template<typename ElemType>
  bool delete_object(const std::string &segment_id,
                     const std::vector<std::string> &object_ids){
    try {
      const boost::interprocess::managed_shared_memory& segment =
          get_segment_manager(segment_id);
      bool ret = true;
      for(std::vector<std::string>::const_iterator obj_it = object_ids.begin();
          obj_it != object_ids.end() ; ++obj_it)
      {
        ret = ret && segment.destroy<ElemType>(obj_it->c_str());
      }
      return ret;
    } catch (const boost::interprocess::interprocess_exception&) {
       return false;
    }
  }

  boost::interprocess::interprocess_mutex* get_segment_mutex(
      const std::string segment_id)
  {
    boost::interprocess::interprocess_mutex* mutex =
        static_cast<boost::interprocess::interprocess_mutex*>(
          get_segment(segment_id).objects_["segment_mutex"]);
  }

  template<typename T>
  void set(const std::string &segment_id,
           const std::string &object_id,
           const T *set_,
           const std::size_t size)
  {
    try {
      const boost::interprocess::managed_shared_memory& segment =
          get_segment_manager(segment_id);

      mutex.lock();
      T* object = segment.find_or_construct<T>(object_id.c_str())[size]();
      for(int i=0;i<size;i++) object[i]=set_[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
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


  void get(const std::string &segment_id,
           const std::string &object_id,
           std::string &get_){

    get_.clear();

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
//      std::pair<char*, std::size_t> object = segment.find<char>(object_id.c_str());
//      for(unsigned i=0; i < object.second ; i++) get_+= (object.first)[i];
//      mutex.unlock();

//    } catch (const boost::interprocess::bad_alloc& e){
//      throw shared_memory::Allocation_exception(segment_id,object_id);

//    } catch (const boost::interprocess::interprocess_exception &e){
//      return;
//    }


  }

}
