// Copyright 2019 Max Planck Gesellschaft and New York University
// Authors: Vincent Berenz, Maximilien Naveau

#pragma once

#include <memory>  // defines the unique_ptr

#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "shared_memory/mutex.hpp"
#include "shared_memory/shared_memory.hpp"

namespace shared_memory
{
typedef boost::interprocess::named_condition SHMCondition;
typedef boost::interprocess::scoped_lock<SHMMutex> SHMScopeLock;

typedef boost::interprocess::interprocess_mutex UnamedSHMMutex;
typedef boost::interprocess::interprocess_condition UnamedSHMCondition;
typedef boost::interprocess::scoped_lock<UnamedSHMMutex> UnamedSHMLock;

/**
 * @brief The LockedConditionVariable class is here as a anonymous layer on top
 * of the boost intersprocess condition variable labrary.
 * It creates a condition variable in a shared memory automatically.
 */
class LockedConditionVariable
{
public:
    /**
     * @brief A condition variable shared over the memory
     * The condition variable is cleaned from the memory
     * on destruction if clean_memory_on_destruction
     * is set to true. Contrary to shared_memory::ConditionVariable,
     * instances of this class manages their mutex and lock internally,
     * with the consequence the mutex can be locked and unlocked exclusively
     * through other instances of LockedConditionVariable.
     */
    LockedConditionVariable(const std::string object_id,
                            bool clean_memory_on_destruction = true);

    ~LockedConditionVariable();

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
     * @brief wait waits until another thread notifies this object
     */
    void wait();

    /**
     * @brief timed_wait wait a notify during a certain certain time and then
     * wake up
     * @param wait_duration in microsecond
     * @return true: the condition variable has been notified, false otherwize
     */
    bool timed_wait(long wait_nano_seconds);

    /**
     * @brief try_lock Tries to acquire the lock without waiting.
     * @return true if manages to acquire the lock, false otherwise.
     * @throws: Exception if the lock is already locked.
     */
    bool try_lock();

    /**
     * @brief unlock Unlocks the lock.
     * @throws: Throws an exception if the lock is not locked.
     */
    void unlock();

    bool owns();

    /**
     * @brief lock_scope this function is used to lock the part of the code
     * that needs protection. It locks the mutex until unlock_scope is used
     */
    void lock_scope();

    /**
     * @brief unlock_scope this function unlock the mutex so remove the
     * protection of the code
     */
    void unlock_scope();

public:
    /**
     * @brief LockedConditionVariable clean their shared memory on destruction.
     * But the destructor may have failed to be called if for some reason
     * the program crashed.
     */
    static void clean(const std::string segment_id);

private:
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
    SHMCondition* condition_variable_;

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

}  // namespace shared_memory
