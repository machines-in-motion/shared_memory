/**
 * @file exchange_manager_demo.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Demonstrate the use of the exhange manager
 */
#include "shared_memory/demos/four_int_values.hpp"
#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/exchange_manager_producer.hpp"

using namespace shared_memory;
#define SEGMENT_ID "exchange_manager_segment"
#define OBJECT_ID "exchange_manager_object"
#define MAX_EXCHANGE_SIZE 10

int main()
{
    Exchange_manager_producer<Four_int_values> producer(
        SEGMENT_ID, OBJECT_ID, MAX_EXCHANGE_SIZE);

    Exchange_manager_consumer<Four_int_values> consumer(
        SEGMENT_ID, OBJECT_ID, MAX_EXCHANGE_SIZE);
}
