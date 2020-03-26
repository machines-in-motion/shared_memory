// Copyright 2019 Max Planck Gesellschaft and New York University
// Authors: Vincent Berenz, Maximilien Naveau

#pragma once

#include <boost/interprocess/sync/scoped_lock.hpp>
#include "shared_memory/mutex.hpp"

namespace shared_memory
{
typedef boost::interprocess::scoped_lock<SHMMutex> SHMScopeLock;

// forward declaration for using the
// friend keyword below
class ConditionVariable;

/**
 * @brief A scope lock object for locking a
 * shared memory mutex, to use for example with
 * a shared memory condition variable.
 * The scope is unlocked on destruction.
 */
class Lock
{
public:
    /**
     * @brief lock the mutex
     */
    Lock(Mutex &mutex);

private:
    // condition variable will access
    // SHMScopeLock directly to lock it
    friend ConditionVariable;
    SHMScopeLock lock_;
};

}  // namespace shared_memory
