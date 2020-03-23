// Copyright Max Planck Gesellschaft and New York University
// Vincent Berenz

#pragma once

#include <boost/interprocess/sync/named_mutex.hpp>
#include <memory>
#include <shared_memory/shared_memory.hpp>

namespace shared_memory
{
typedef boost::interprocess::named_mutex SHMMutex;

// forward declaration for usage of
// the friend keyword
class Lock;

class Mutex
{
public:
    /**
     * @brief A Mutex accessible to several processes via the shared memory
     * The mutex is cleaned from the shared memory on destruction if
     * clean_memory_on_destruction is true (the default)
     */
    Mutex(std::string mutex_id, bool clean_memory_on_destruction = true);

    ~Mutex();

    /**
     * @brief lock the mutex
     */
    void lock();

    /**
     * @brief unlock the mutex
     */
    void unlock();

public:
    static void clean(std::string mutex_id);

private:
    // Lock will access directly SHMMutex to lock it
    friend Lock;

    std::string mutex_id_;
    SHMMutex mutex_;
    bool clean_memory_on_destruction_;
};

}  // namespace shared_memory
