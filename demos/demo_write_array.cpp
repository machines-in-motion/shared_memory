/**
 * @file demo_write_array.cpp
 * @author Vincent Berenz
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief example of writing into interprocesses arrays
 *
 */

#include <signal.h>
#include <unistd.h>
#include "shared_memory/array.hpp"
#include "shared_memory/demos/item.hpp"

#define SIZE 4
#define SEGMENT_SERIALIZED "demo_array_serialized"
#define SEGMENT_FUNDAMENTAL "demo_array_fundamental"
#define SEGMENT_FUNDAMENTAL_ARRAY "demo_array_fundamental_array"

static bool RUNNING = true;

void stop(int)
{
    RUNNING = false;
}

/**
 * @brief create interprocesses arrays and write into them. Run demo_read_array
 * for a process reading these arrays
 */
void run()
{
    // in case a previous run did not exit properly
    shared_memory::clear_array(SEGMENT_SERIALIZED);
    shared_memory::clear_array(SEGMENT_FUNDAMENTAL);
    shared_memory::clear_array(SEGMENT_FUNDAMENTAL_ARRAY);

    // it will be safe to have another process
    // reading/writting in the same array
    bool mutex_protected = true;

    // array declared in this files
    // will wipe the shared memory on exit
    bool clear_on_destruction = true;

    // array of SIZE Item (Item being themselfs of size SIZE)
    // Item implements a serialize function, and instances
    // of items will be serialized into a string before being
    // written in the shared memory
    // see include/shared_memory/demos/item.hpp
    shared_memory::array<shared_memory::Item<SIZE> > serialized(
        SEGMENT_SERIALIZED, SIZE, mutex_protected, clear_on_destruction);

    // array of SIZE integer
    shared_memory::array<int> fundamental(
        SEGMENT_FUNDAMENTAL, SIZE, mutex_protected, clear_on_destruction);

    // array of array : SIZE * SIZE
    shared_memory::array<int, SIZE> fundamental_array(
        SEGMENT_FUNDAMENTAL_ARRAY, SIZE, mutex_protected, clear_on_destruction);

    int count = 0;

    shared_memory::Item<SIZE> item(0);

    int values[SIZE];

    while (RUNNING)
    {
        for (int i = 0; i < SIZE; i++)
        {
            // writting items in serialized
            item.fill(0);
            item.set(i, count);
            item.compact_print();
            serialized.set(i, item);

            // writting integers in fundamental
            fundamental.set(i, count);

            // writting values in fundamental arrays
            for (int j = 0; j < SIZE; j++)
            {
                values[j] = 0;
            }
            values[i] = count;
            fundamental_array.set(i, values);
        }

        std::cout << std::endl;
        count++;
        if (count == 10)
        {
            count = 0;
        }
        usleep(100000);
    }
}

int main()
{
    // stop on ctrl+c
    struct sigaction cleaning;
    cleaning.sa_handler = stop;
    sigemptyset(&cleaning.sa_mask);
    cleaning.sa_flags = 0;
    sigaction(SIGINT, &cleaning, nullptr);

    run();
}
