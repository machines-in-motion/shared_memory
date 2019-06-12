#include "shared_memory/thread_synchronisation.hpp"

namespace shared_memory {

  
  ConditionVariable::ConditionVariable(const std::string segment_id,
				       const std::string object_id,
				       bool clean_memory_on_destruction):
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
    {}

  
  ConditionVariable::~ConditionVariable()
  {
    
    unlock_scope();
      
    if (clean_memory_on_destruction_){
      boost::interprocess::named_mutex::remove(mutex_id_.c_str());
      boost::interprocess::named_condition::remove(condition_id_.c_str());
    }
      
  }

  
  void ConditionVariable::notify_all()
  {
    condition_variable_.notify_all();
  }

  
  void ConditionVariable::notify_one()
  {
    condition_variable_.notify_one();
  }


  void ConditionVariable::wait()
  {
    if(lock_)
      {
        condition_variable_.wait(*lock_);
      } else {
      std::cout << "ConditionVariable::wait(): "
		<< "WARNING, undefined behavior, the scope has not been locked"
		<< std::endl;
    }
  }


  bool ConditionVariable::timed_wait(long wait_nano_seconds)
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


  bool ConditionVariable::try_lock()
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


  void ConditionVariable::unlock()
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

  
  bool ConditionVariable::owns()
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


  void ConditionVariable::lock_scope()
  {
    lock_.reset(new SHMScopeLock(mutex_));
  }


  void ConditionVariable::unlock_scope()
  {
    lock_.reset(nullptr);
  }


}
