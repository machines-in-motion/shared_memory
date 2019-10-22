#include "shared_memory/condition_variable.hpp"



namespace shared_memory {


  ConditionVariable::ConditionVariable(const std::string object_id,
				       bool clean_memory_on_destruction):

    condition_id_(object_id),
    clean_memory_on_destruction_(clean_memory_on_destruction)
    {
      // note: using an instance that is defined in initializer list
      //       sometimes results in segfault. Thus switching to pointer.
      condition_variable_ = new SHMCondition(boost::interprocess::open_or_create,
					    condition_id_.c_str());
    }

  
  ConditionVariable::~ConditionVariable()
  {
    delete(condition_variable_);
    if (clean_memory_on_destruction_){
      boost::interprocess::named_condition::remove(condition_id_.c_str());
    }
  }

  void ConditionVariable::clean(const std::string object_id){
    boost::interprocess::named_condition::remove(object_id.c_str());
  }
  
  
  void ConditionVariable::notify_all()
  {
    condition_variable_->notify_all();
  }

  
  void ConditionVariable::notify_one()
  {
    condition_variable_->notify_one();
  }


  void ConditionVariable::wait(Lock &lock)
  {
    // ConditionVariable is friend of Lock
    condition_variable_->wait(lock.lock_);
  }


  bool ConditionVariable::timed_wait(Lock &lock, long wait_nano_seconds)
  {
    boost::posix_time::ptime current_time =
      boost::interprocess::microsec_clock::universal_time();
    boost::posix_time::time_duration waiting_time =
      boost::posix_time::microseconds(static_cast<long>(
							static_cast<double>(wait_nano_seconds)*0.001));
    return condition_variable_->timed_wait(lock.lock_,
					   current_time + waiting_time);
  }



}
