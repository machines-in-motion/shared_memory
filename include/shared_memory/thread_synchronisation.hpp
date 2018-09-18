#ifndef THREAD_SYNCHRONISATION_HPP
#define THREAD_SYNCHRONISATION_HPP

#include <memory> // defines the unique_ptr

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/chrono.hpp>

#include <shared_memory/shared_memory.hpp>

namespace shared_memory {

  typedef boost::interprocess::named_mutex SHMMutex;
  typedef boost::interprocess::named_condition SHMCondition;
  typedef boost::interprocess::scoped_lock<SHMMutex> SHMScopeLock;

  typedef boost::interprocess::interprocess_mutex UnamedSHMMutex;
  typedef boost::interprocess::interprocess_condition UnamedSHMCondition;
  typedef boost::interprocess::scoped_lock<UnamedSHMMutex> UnamedSHMLock;



  struct Mutex{
    Mutex(std::string mutex_id):mutex_{
              boost::interprocess::open_or_create,
              mutex_id.c_str()}
    {
      mutex_id_ = mutex_id;
    }

    ~Mutex()
    {
      boost::interprocess::named_mutex::remove(mutex_id_.c_str());
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
  };

  class ConditionVariable
  {
  public:
    ConditionVariable(const std::string segment_id,
                       const std::string object_id):
      segment_id_(segment_id)
      ,mutex_id_(object_id + "_mtx")
      ,condition_id_(object_id + "_cond")
      ,mutex_{
        boost::interprocess::open_or_create,
        mutex_id_.c_str()
      }
      ,condition_variable_{
        boost::interprocess::open_or_create,
        condition_id_.c_str()
      }
    {
      lock_ = SHMScopeLock(mutex_);
    }

    ~ConditionVariable()
    {
      boost::interprocess::named_mutex::remove(mutex_id_.c_str());
      boost::interprocess::named_condition::remove(condition_id_.c_str());
    }

    void notify_all()
    {
      condition_variable_.notify_all();
    }

    void notify_one()
    {
      condition_variable_.notify_one();
    }

    void wait()
    {
      condition_variable_.wait(lock_);
    }

    /**
     * @brief timed_wait wait a notify during a certain certain time and then
     * wake up
     * @param wait_duration in microsecond
     */
    bool timed_wait(unsigned wait_micro_seconds)
    {
      boost::posix_time::ptime current_time =
          boost::posix_time::microsec_clock::universal_time();
      boost::posix_time::time_duration waiting_time =
          boost::posix_time::microseconds(wait_micro_seconds);
      return condition_variable_.timed_wait(lock_, current_time + waiting_time);
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

    UnamedSHMCondition cond_var_;

    SHMScopeLock lock_;
  };
}

#endif // THREAD_SYNCHRONISATION_HPP
