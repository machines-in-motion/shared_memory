/**
 * @file shared_memory.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Implementation of the shared_memory.hpp API
 */
#include "shared_memory/shared_memory.hpp"

namespace bi = boost::interprocess;

namespace shared_memory
{
static uint SEGMENT_SIZE = DEFAULT_SHARED_MEMORY_SIZE;
static std::mutex SEGMENT_SIZE_MUTEX;

void set_segment_sizes(uint multiplier_1025)
{
    SEGMENT_SIZE_MUTEX.lock();
    SEGMENT_SIZE = multiplier_1025 * 1025;
    SEGMENT_SIZE_MUTEX.unlock();
}

void set_default_segment_sizes()
{
    SEGMENT_SIZE_MUTEX.lock();
    SEGMENT_SIZE = DEFAULT_SHARED_MEMORY_SIZE;
    SEGMENT_SIZE_MUTEX.unlock();
}

bool VERBOSE = false;
void set_verbose(bool mode)
{
    VERBOSE = mode;
}

  /*
static std::chrono::microseconds time_now()
{
    std::chrono::steady_clock::time_point now = Clock::now();
    std::chrono::microseconds m =
        std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return m;
}
  
bool GET_CREATES_SEGMENT = true;  
void wait_for_segment(const string& segment_id, int timeout_us)
  {
    GET_CREATES_SEGMENT = false;
    std::chrono::microseconds start = time_now();
    std::chrono::microseconds timeout{timeout_is};
    bool created=false;
    while(!created)
      {
	try
	  {
	    get_segment(segment_id,false);
	  }
	catch (Non_existing_segment)
	  {
	    usleep(5);
	  }
	if(time_now()-start>timeout)
	  {
	    GET_CREATES_SEGMENT = true;
	    throw Non_existing_segment(segment_id);
	  }
      }
    GET_CREATES_SEGMENT = true;
  }*/
  
/***********************************************
 * Definition of the SharedMemorySegment class *
 ***********************************************/

SharedMemorySegment::SharedMemorySegment(std::string segment_id,
                                         bool clear_upon_destruction,
					 bool create)
  : ravioli_(-1)
{
    // save the id the of the segment
    segment_id_ = segment_id;

    // check if we should delete the memory upon destruction.
    clear_upon_destruction_ = clear_upon_destruction;

    std::cout << "shared memory segment 1" << std::endl;
    
    // create and/or map the memory segment
    SEGMENT_SIZE_MUTEX.lock();
    if(create)
      {
	std::cout << "shared memory segment 2" << std::endl;
	
	segment_manager_ =
	  boost::interprocess::managed_shared_memory(
						     boost::interprocess::open_or_create,
						     segment_id.c_str(), SEGMENT_SIZE);
	std::cout << "shared memory segment 3" << std::endl;
      }
    else
      {
	ravioli_+=1;
	std::cout << "shared memory segment 4 "<< ravioli_ << std::endl;;
	try
	  {
	    segment_manager_ =
	      boost::interprocess::managed_shared_memory(
							 boost::interprocess::open_only,
							 segment_id.c_str());
	  }
	catch(...)
	  {
	    std::cout << "banana !" << std::endl;
	  }
	std::cout << "shared memory segment 5 " << ravioli_ << std::endl;
	std::cout << "shared_memory segment 6 " << segment_manager_.check_sanity() << "" << std::endl;
      }
	SEGMENT_SIZE_MUTEX.unlock();
    create_mutex();
}

void SharedMemorySegment::clear_memory()
{
    boost::interprocess::shared_memory_object::remove(segment_id_.c_str());
}

void SharedMemorySegment::get_object(const std::string &object_id,
                                     std::string &get_)
{
    mutex_->lock();

    register_object_read_only<char>(object_id);

    get_ = std::string(static_cast<char *>(objects_[object_id].first),
                       objects_[object_id].second);

    mutex_->unlock();
}

SharedMemorySegment &get_segment(const std::string &segment_id,
                                 const bool clear_upon_destruction,
				 const bool create)
{
  std::cout << "get segment 1" << std::endl;
    if (GLOBAL_SHM_SEGMENTS.count(segment_id) == 0)
    {
      std::cout << "get segment 2" << std::endl;
      GLOBAL_SHM_SEGMENTS[segment_id].reset(
					    new SharedMemorySegment(segment_id,
								    clear_upon_destruction,
								    create));
      std::cout << "get segment 3" << std::endl;
    }
    std::cout << "get segment 4" << std::endl;
    GLOBAL_SHM_SEGMENTS[segment_id]->set_clear_upon_destruction(
        clear_upon_destruction);
    std::cout << "get segment 5" << std::endl;
    return *GLOBAL_SHM_SEGMENTS[segment_id];
}

  SegmentInfo get_segment_info(const std::string &segment_id)
{
  std::cout << "get segment info 1" << std::endl;
    SharedMemorySegment &segment =
      get_segment(segment_id, false, false);
    std::cout << "get segment info 2" << std::endl;
    SegmentInfo si = segment.get_info();
    std::cout << "get segment info 3" << std::endl;
    return si;
}

bool segment_exists(const std::string &segment_id)
{
    if (GLOBAL_SHM_SEGMENTS.count(segment_id) == 0)
    {
        return false;
    }
    return true;
}

boost::interprocess::interprocess_mutex &get_segment_mutex(
    const std::string segment_id)
{
    SharedMemorySegment &segment = get_segment(segment_id);
    return *segment.mutex_;
}

void delete_segment(const std::string &segment_id)
{
    // here the unique pointer destroy the object for us.
    GLOBAL_SHM_SEGMENTS.erase(segment_id);
}

void delete_all_segment()
{
    for (SegmentMap::iterator seg_it = GLOBAL_SHM_SEGMENTS.begin();
         seg_it != GLOBAL_SHM_SEGMENTS.end();
         seg_it = GLOBAL_SHM_SEGMENTS.begin())
    {
        get_segment(seg_it->second->get_segment_id(), true);
        GLOBAL_SHM_SEGMENTS.erase(seg_it);
    }
}

void clear_shared_memory(const std::string &segment_id)
{
    boost::interprocess::shared_memory_object::remove(segment_id.c_str());
    delete_segment(segment_id);
}

/***********************************
 * Definition of all set functions *
 ***********************************/

void set(const std::string &segment_id,
         const std::string &object_id,
         const std::string &set_)
{
    set<char>(segment_id, object_id, set_.c_str(), set_.size());
}

/***********************************
 * Definition of all get functions *
 ***********************************/

void get(const std::string &segment_id,
         const std::string &object_id,
         std::string &get_,
	 bool create)
{
    try
    {
      SharedMemorySegment &segment = get_segment(segment_id,false,create);
        segment.get_object(object_id, get_);
    }
    catch (const boost::interprocess::bad_alloc &)
    {
        throw shared_memory::Allocation_exception(segment_id, object_id);
    }
    catch (const boost::interprocess::interprocess_exception &)
    {
        return;
    }
}

}  // namespace shared_memory
