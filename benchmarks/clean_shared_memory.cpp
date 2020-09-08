/**
 * @file clean_shared_memory.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Clean the shared memory of the benchmark, the unnittests, ...
 */

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include "shared_memory/benchmarks/benchmark_common.hh"

int main()
{
    boost::interprocess::named_mutex::remove(SHM_NAME.c_str());
    boost::interprocess::shared_memory_object::remove(SHM_OBJECT_NAME.c_str());
    boost::interprocess::shared_memory_object::remove("main_memory");
    return 0;
}
