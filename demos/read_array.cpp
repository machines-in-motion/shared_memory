/**
 * @file demo_read_array.cpp
 * @author Vincent Berenz
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief example of reading from interprocesses arrays
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

void print_array(int* values, int size)
{
    for (int i = 0; i < size; i++)
    {
        std::cout << values[i] << " ";
    }
}

/**
 * @brief read from the array created by demo_write_array, which
 * should have been started before this demo was
 * (infinite hanging expected otherwise)
 */

void run()
{
    bool mutex_protected = true;

    // because done in demo_write_array
    bool clear_on_destruction = false;

    shared_memory::array<shared_memory::Item<SIZE> > serialized(
	SEGMENT_SERIALIZED, SIZE, clear_on_destruction, mutex_protected);

    shared_memory::array<int> fundamental(
        SEGMENT_FUNDAMENTAL, SIZE, clear_on_destruction, mutex_protected);

    shared_memory::array<int, SIZE> fundamental_array(
        SEGMENT_FUNDAMENTAL_ARRAY, SIZE, clear_on_destruction, mutex_protected);

    shared_memory::Item<SIZE> item;
    int values[SIZE];
    int value;

    while (RUNNING)
    {
        for (int i = 0; i < SIZE; i++)
        {
            serialized.get(i, item);
            fundamental.get(i, value);
            fundamental_array.get(i, values);
            item.compact_print();
            std::cout << " | " << value << " | ";
            print_array(values, SIZE);
            std::cout << " | ";
        }

        std::cout << std::endl;
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
