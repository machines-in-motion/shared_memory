#include "shared_memory/locked_condition_variable.hpp"


namespace shared_memory {

  
  LockedConditionVariable::LockedConditionVariable(const std::string segment_id,
				       const std::string object_id,
				       bool clean_memory_on_destruction):
    segment_id_(segment_id)
    ,mutex_id_(object_id + "_mtx")
    ,condition_id_(object_id + "_cond")
    ,mutex_{
    boost::interprocess::open_or_create,
      mutex_id_.c_str()
      }
    ,clean_memory_on_destruction_(clean_memory_on_destruction)
    {
      condition_variable_ = new SHMCondition(boost::interprocess::open_or_create,
					     condition_id_.c_str());
    }
  
  LockedConditionVariable::~LockedConditionVariable()
  {
    
    unlock_scope();
    if(condition_variable_!=nullptr)
      {
	delete(condition_variable_);
      }
    if (clean_memory_on_destruction_)
      {
	boost::interprocess::named_mutex::remove(mutex_id_.c_str());
	boost::interprocess::named_condition::remove(condition_id_.c_str());
      }
      
  }

  void LockedConditionVariable::clean(const std::string segment_id,
				const std::string object_id){

    LockedConditionVariable cv(segment_id,object_id,true);
    
  }
  
  
  void LockedConditionVariable::notify_all()
  {
    condition_variable_->notify_all();
  }

  
  void LockedConditionVariable::notify_one()
  {
    condition_variable_->notify_one();
  }


  void LockedConditionVariable::wait()
  {
    if(lock_)
      {
        condition_variable_->wait(*lock_);
      } else {
      std::cout << "LockedConditionVariable::wait(): "
		<< "WARNING, undefined behavior, the scope has not been locked"
		<< std::endl;
    }
  }


  bool LockedConditionVariable::timed_wait(long wait_nano_seconds)
  {
    if(lock_)
      {
        boost::posix_time::ptime current_time =
	  boost::interprocess::microsec_clock::universal_time();
        boost::posix_time::time_duration waiting_time =
	  boost::posix_time::microseconds(static_cast<long>(
							    static_cast<double>(wait_nano_seconds)*0.001));
        return condition_variable_->timed_wait(*lock_,
                                              current_time + waiting_time);
      }
    std::cout << "LockedConditionVariable::timed_wait(): "
	      << "WARNING, undefined behavior, the scope has not been locked"
	      << std::endl;
    return false;
  }


  bool LockedConditionVariable::try_lock()
  {
    if(lock_)
      {
        return lock_->try_lock();
      } else {
      std::cout << "LockedConditionVariable::try_lock(): "
		<< "WARNING, undefined behavior, the scope has not been locked"
		<< std::endl;
      return false;
    }
  }


  void LockedConditionVariable::unlock()
  {
    if(lock_)
      {
        lock_->unlock();
      } else {
      std::cout << "LockedConditionVariable::try_lock(): "
		<< "WARNING, undefined behavior, the scope has not been locked"
		<< std::endl;
    }
  }

  
  bool LockedConditionVariable::owns()
  {
    if (lock_)  {
      return lock_->owns();
    } else {
      std::cout << "LockedConditionVariable::owns(): "
		<< "WARNING, undefined behavior, the scope has not been locked"
		<< std::endl;
      return false;
    }
  }


  void LockedConditionVariable::lock_scope()
  {
    lock_.reset(new SHMScopeLock(mutex_));
  }


  void LockedConditionVariable::unlock_scope()
  {
    lock_.reset(nullptr);
  }


}
