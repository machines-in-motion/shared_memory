#include "shared_memory/shared_memory.hpp"

namespace bi=boost::interprocess;

namespace shared_memory {

  /***********************************************
   * Definition of the SharedMemorySegment class *
   ***********************************************/

  SharedMemorySegment::SharedMemorySegment(std::string segment_id,
                                           bool clear_upon_destruction)
//    : named_mtx_{boost::interprocess::open_or_create,
//                 (segment_id_ + "_mutex").c_str()}
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
    create_mutex();
  }

  void SharedMemorySegment::clear_memory()
  {
    boost::interprocess::shared_memory_object::remove(segment_id_.c_str());
  }

  void SharedMemorySegment::get_object(const std::string& object_id,
                                       std::string& get_)
  {
    mutex_->lock();

    register_object_read_only<char>(object_id);

    get_ = std::string(static_cast<char*>(objects_[object_id].first),
                       objects_[object_id].second);

    mutex_->unlock();
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
    GLOBAL_SHM_SEGMENTS[segment_id]->set_clear_upon_destruction(
          clear_upon_destruction);
    return *GLOBAL_SHM_SEGMENTS[segment_id];
  }

  boost::interprocess::interprocess_mutex& get_segment_mutex(
      const std::string segment_id)
  {
    SharedMemorySegment& segment = get_segment(segment_id);
    return *segment.mutex_;
  }

  void delete_segment(const std::string &segment_id){
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

  void clear_shared_memory(const std::string& segment_id)
  {
    boost::interprocess::shared_memory_object::remove(segment_id.c_str());
    delete_segment(segment_id);
  }

  /***********************************
   * Definition of all set functions *
   ***********************************/

  void set(const std::string &segment_id,
           const std::string &object_id,
           const std::string &set_){
    set<char>(segment_id,
              object_id,
              set_.c_str(),
              set_.size());
  }

  /***********************************
   * Definition of all get functions *
   ***********************************/

  void get(const std::string &segment_id,
           const std::string &object_id,
           std::string &get_){
    try {
      SharedMemorySegment& segment = get_segment(segment_id);
      segment.get_object(object_id, get_);
    } catch (const boost::interprocess::bad_alloc&){
      throw shared_memory::Allocation_exception(segment_id,object_id);
    }catch (const boost::interprocess::interprocess_exception&){
      return;
    }
  }

} // namespace shared_memory
