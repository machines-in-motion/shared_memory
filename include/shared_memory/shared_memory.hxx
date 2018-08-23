#include <shared_memory/shared_memory.hpp>

namespace bi=boost::interprocess;
namespace shared_memory {

  /**************************************************************
   * Definition of all methods of the SharedMemorySegment class *
   **************************************************************/
  SharedMemorySegment::SharedMemorySegment(std::string name)
  {
    name_ = name;
  }

  SharedMemorySegment::~SharedMemorySegment()
  {
    for(SHMObjects::iterator it=objects_.begin() ; it!=objects_.end() ; ++it)
    {
      boost::interprocess::shared_memory_object::remove(it->first.c_str());
    }
  }

  /**
   * @brief GLOBAL_SHARED_MEMORY_SEGMENT is global variable that acts as a
   * a shared memory manager.
   *
   * The use of the std::unique_ptr allows to delete the object and re-create
   * at will.
   */
  static std::map<std::string, std::unique_ptr<SharedMemorySegment> >
    GLOBAL_SHM_SEGMENTS;

  void delete_segment(const std::string &segment_id){
    GLOBAL_SHM_SEGMENTS[segment_id].reset(nullptr);
  }

  void delete_object(){

  }

  void clear_mutexes(const std::vector<std::string> &mutexes){
    for(const std::string& str: mutexes){
      boost::interprocess::named_mutex::remove(str.c_str());
      std::string keys_object_id = std::string("key_")+str;
      std::string values_object_id = std::string("values_")+str;
      std::string sizes_object_id = std::string("sizes_")+str;
      boost::interprocess::named_mutex::remove(keys_object_id.c_str());
      boost::interprocess::named_mutex::remove(values_object_id.c_str());
    }
  }


  void clear(const std::string &segment_id,
             const std::vector<std::string> &mutexes){
    clear_mutexes(mutexes);
    clear_segment(segment_id);
  }

  void clear(const std::string &segment_id,
             const std::vector<std::string> &mutexes,
             const std::map<std::string,std::vector<std::string>> &map_keys){

    clear_mutexes(mutexes);

    std::vector<std::string> mutexes_;

    for(auto& key_values: map_keys){
      for(std::string key: key_values.second){
        std::string object_id = key_values.first+"_"+key;
        mutexes_.push_back(object_id);
      }
    }

    clear_mutexes(mutexes_);

    clear_segment(segment_id);

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

    try {

      boost::interprocess::managed_shared_memory segment{
        boost::interprocess::open_or_create,
        segment_id.c_str(),
        _SHARED_MEMORY_SIZE
      };
      boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create,
        object_id.c_str()
      };

      mutex.lock();
      std::pair<char*, std::size_t> object = segment.find<char>(object_id.c_str());
      for(unsigned i=0; i < object.second ; i++) get_+= (object.first)[i];
      mutex.unlock();

    } catch (const boost::interprocess::bad_alloc& e){
      throw shared_memory::Allocation_exception(segment_id,object_id);

    } catch (const boost::interprocess::interprocess_exception &e){
      return;
    }


  }

}
