#include "shared_memory/mutex.hpp"

namespace shared_memory
{
Mutex::Mutex(std::string mutex_id, bool clean_memory_on_destruction)
    : mutex_{boost::interprocess::open_or_create, mutex_id.c_str()}
{
    mutex_id_ = mutex_id;
    clean_memory_on_destruction_ = clean_memory_on_destruction;
}

Mutex::~Mutex()
{
    mutex_.unlock();
    if (clean_memory_on_destruction_)
    {
        boost::interprocess::named_mutex::remove(mutex_id_.c_str());
    }
    else
    {
        try
        {
            mutex_.unlock();
        }
        catch (...)
        {
        }
    }
}

void Mutex::lock()
{
    mutex_.lock();
}

void Mutex::unlock()
{
    mutex_.unlock();
}

void Mutex::clean(std::string mutex_id)
{
    Mutex mutex(mutex_id, true);
}

}  // namespace shared_memory
