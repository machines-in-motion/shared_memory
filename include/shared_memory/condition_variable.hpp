// Copyright 2019 Max Planck Gesellschaft and New York University
// Authors: Vincent Berenz, Maximilien Naveau

#pragma once

#include "shared_memory/lock.hpp"

#include <boost/interprocess/sync/named_condition.hpp>

namespace shared_memory
{
typedef boost::interprocess::named_condition SHMCondition;

class ConditionVariable
{
public:
    /**
     * @brief A condition variable shared over the memory
     * The condition variable is cleaned from the memory
     * on destruction if clean_memory_on_destruction
     * is set to true
     */
    ConditionVariable(const std::string object_id,
                      bool clean_memory_on_destruction);

    ~ConditionVariable();

    /**
     * @brief notify_all is notifying all condition variables with the same
     * mutex
     */
    void notify_all();

    /**
     * @brief notify_one notifies one condition variable with the same mutex
     */
    void notify_one();

    /**
     * @brief timed_wait wait a notify during a certain certain time and then
     * wake up
     * @param wait_duration in microsecond
     * @return true: the condition variable has been notified, false otherwize
     */
    bool timed_wait(Lock &lock, long wait_nano_seconds);

    /**
     * @brief wait waits until another thread notifies this object
     */
    void wait(Lock &lock);

public:
    static void clean(const std::string object_id);

private:
    /**
     * @brief condition_id_ is the condition variable name in the shared memory
     */
    std::string condition_id_;

    /**
     * @brief if true (the default), clean the shared memory of the
     * hosted mutex and condition.
     */
    bool clean_memory_on_destruction_;

    /**
     * @brief condition_variable_ is the boost condition variable that is used
     */
    SHMCondition *condition_variable_;

    /**
     * @brief mutex_id_ is the mutex name in the shared memory
     */
    std::string mutex_id_;
};

}  // namespace shared_memory
