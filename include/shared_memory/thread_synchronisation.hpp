#ifndef THREAD_SYNCHRONISATION_HPP
#define THREAD_SYNCHRONISATION_HPP

#include <memory> // defines the unique_ptr

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <shared_memory/shared_memory.hpp>

namespace shared_memory {

  typedef boost::interprocess::named_mutex SHMMutex;
  typedef boost::interprocess::named_condition SHMCondition;
  typedef boost::interprocess::scoped_lock<SHMMutex> SHMScopeLock;

  typedef boost::interprocess::interprocess_mutex UnamedSHMMutex;
  typedef boost::interprocess::interprocess_condition UnamedSHMCondition;
  typedef boost::interprocess::scoped_lock<UnamedSHMMutex> UnamedSHMLock;



  struct Mutex{
    Mutex(std::string mutex_id, bool clean_memory_on_destruction=true ):mutex_{
              boost::interprocess::open_or_create,
              mutex_id.c_str()}
    {

      mutex_id_ = mutex_id;
      clean_memory_on_destruction_ = clean_memory_on_destruction;
    }

    ~Mutex()
    {
      if(clean_memory_on_destruction_){
	boost::interprocess::named_mutex::remove(mutex_id_.c_str());
      } else {
	try {
	  mutex_.unlock();
	} catch(...) {}
      }
    }

    void lock()
    {
      mutex_.lock();
    }

    void unlock()
    {
      mutex_.unlock();
    }

    std::string mutex_id_;
    SHMMutex mutex_;
    bool clean_memory_on_destruction_;
    
  };

  /**
   * @brief The ConditionVariable class is here as a anonymous layer on top
   * of the boost intersprocess condition variable labrary.
   * It creates a condition variable in a shared memory automatically.
   */
  class ConditionVariable
  {
  public:
    ConditionVariable(const std::string segment_id,
		      const std::string object_id,
		      bool clean_memory_on_destruction=true):
      segment_id_(segment_id)
      ,mutex_id_(object_id + "_mtx")
      ,condition_id_(object_id + "_cond")
      ,clean_memory_on_destruction_(clean_memory_on_destruction)
      ,mutex_{
        boost::interprocess::open_or_create,
        mutex_id_.c_str()
      }
      ,condition_variable_{
        boost::interprocess::open_or_create,
        condition_id_.c_str()
      }
    {
    }

    /**
      * @brief ~ConditionVariable is the destructor of the class, his job is to
      * remove the condition variable from the shared memory
      */
    ~ConditionVariable()
    {
      unlock_scope();
      
      if (clean_memory_on_destruction_){
	boost::interprocess::named_mutex::remove(mutex_id_.c_str());
	boost::interprocess::named_condition::remove(condition_id_.c_str());
      }
      
    }

    /**
     * @brief notify_all is notifying all condition variables with the same
     * mutex
     */
    void notify_all()
    {
      condition_variable_.notify_all();
    }

    /**
     * @brief notify_one notifies one condition variable with the same mutex
     */
    void notify_one()
    {
      condition_variable_.notify_one();
    }

    /**
     * @brief wait waits until another thread notifies this object
     */
    void wait()
    {
      if(lock_)
      {
        condition_variable_.wait(*lock_);
      }else{
        std::cout << "ConditionVariable::wait(): "
                  << "WARNING, undefined behavior, the scope has not been locked"
                  << std::endl;
      }
    }

    /**
     * @brief timed_wait wait a notify during a certain certain time and then
     * wake up
     * @param wait_duration in microsecond
     * @return true: the condition variable has been notified, false otherwize
     */
    bool timed_wait(long wait_nano_seconds)
    {
      if(lock_)
      {
        boost::posix_time::ptime current_time =
            boost::interprocess::microsec_clock::universal_time();
        boost::posix_time::time_duration waiting_time =
            boost::posix_time::microseconds(static_cast<long>(
              static_cast<double>(wait_nano_seconds)*0.001));
        return condition_variable_.timed_wait(*lock_,
                                              current_time + waiting_time);
      }
      std::cout << "ConditionVariable::timed_wait(): "
                << "WARNING, undefined behavior, the scope has not been locked"
                << std::endl;
      return false;
    }

    /**
     * @brief try_lock Tries to acquire the lock without waiting.
     * @return true if manages to acquire the lock, false otherwise.
     * @throws: Exception if the lock is already locked.
     */
    bool try_lock()
    {
      if(lock_)
      {
        return lock_->try_lock();
      } else {
        std::cout << "ConditionVariable::try_lock(): "
                  << "WARNING, undefined behavior, the scope has not been locked"
                  << std::endl;
        return false;
      }
    }

    /**
     * @brief unlock Unlocks the lock.
     * @throws: Throws an exception if the lock is not locked.
     */
    void unlock()
    {
      if(lock_)
      {
        lock_->unlock();
      } else {
        std::cout << "ConditionVariable::try_lock(): "
                  << "WARNING, undefined behavior, the scope has not been locked"
                  << std::endl;
      }
    }

    bool owns()
    {
      if (lock_)  {
        return lock_->owns();
      } else {
        std::cout << "ConditionVariable::owns(): "
                  << "WARNING, undefined behavior, the scope has not been locked"
                  << std::endl;
        return false;
      }
    }

    /**
     * @brief lock_scope this function is used to lock the part of the code
     * that needs protection. It locks the mutex until unlock_scope is used
     */
    void lock_scope()
    {
      lock_.reset(new SHMScopeLock(mutex_));
    }

    /**
     * @brief unlock_scope this function unlock the mutex so remove the
     * protection of the code
     */
    void unlock_scope()
    {
      lock_.reset(nullptr);
    }

  private:
    /**
     * @brief segment_id_ is the segment name
     */
    std::string segment_id_;

    /**
     * @brief mutex_id_ is the mutex name in the shared memory
     */
    std::string mutex_id_;

    /**
     * @brief condition_id_ is the condition variable name in the shared memory
     */
    std::string condition_id_;

    /**
     * @brief shm_segment is the boost object that manages the shared memory
     * segment
     */
    boost::interprocess::managed_shared_memory segment_manager_;

  public:
    /**
     * @brief mutex_ is the mutex associated to the condition variable
     */
    SHMMutex mutex_;

    /**
     * @brief condition_variable_ is the boost condition variable that is used
     */
    SHMCondition condition_variable_;

    /**
     * @brief lock_ is a object that protects the codes with a mutex, see the
     * boost documentation about "boost::interprocess::scoped_lock"
     */
    std::unique_ptr<SHMScopeLock> lock_;

    /**
     * @brief if true (the default), clean the shared memory of the
     * hosted mutex and condition. 
     */
    bool clean_memory_on_destruction_;
    
  };
}

#endif // THREAD_SYNCHRONISATION_HPP
