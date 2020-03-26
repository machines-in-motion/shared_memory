/**
 * @file stress_get_raw_boost_inefficient.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Use the raw boost API in order to compare the efficiency of the new
 * API compare to the standard boost API
 */
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include "shared_memory/benchmarks/benchmark_common.hh"

void cleaning_memory(int)
{
    RUNNING = false;
    boost::interprocess::named_mutex::remove(SHM_NAME.c_str());
}

int main()
{
    boost::interprocess::named_mutex::remove(SHM_NAME.c_str());

    // exiting on ctrl+c
    struct sigaction exiting;
    exiting.sa_handler = cleaning_memory;
    sigemptyset(&exiting.sa_mask);
    exiting.sa_flags = 0;
    sigaction(SIGINT, &exiting, nullptr);

    int count = 0;
    RUNNING = true;
    MeasureTime meas_time;
    while (RUNNING && count < MAX_NUNMBER_OF_ITERATION)
    {
        boost::interprocess::managed_shared_memory segment{
            boost::interprocess::open_or_create,
            SHM_NAME.c_str(),
            SHARED_MEMORY_SIZE};

        boost::interprocess::named_mutex mutex{
            boost::interprocess::open_or_create, SHM_OBJECT_NAME.c_str()};

        mutex.lock();
        std::pair<double*, std::size_t> object =
            segment.find<double>(SHM_OBJECT_NAME.c_str());
        for (size_t i = 0; i < object.second; i++)
        {
            DATA[i] = (object.first)[i];
        }
        mutex.unlock();

        ++count;
        if (count % NUMBER_OR_MEASURED_ITERATIONS == 0)
        {
            meas_time.update();
            std::cout << meas_time << std::endl;
        }
    }
}
