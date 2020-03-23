#include "shared_memory/lock.hpp"

namespace shared_memory
{
Lock::Lock(Mutex &mutex) : lock_(mutex.mutex_)
{
}

}  // namespace shared_memory
