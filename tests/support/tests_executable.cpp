/**
 * @file tests_executable.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Used to generate or read data from the shared memory.
 */
#include <iostream>

#include "shared_memory/condition_variable.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/lock.hpp"
#include "shared_memory/locked_condition_variable.hpp"
#include "shared_memory/mutex.hpp"
#include "shared_memory/shared_memory.hpp"
#include "shared_memory/tests/tests.h"

int main(int, char *argv[])
{
    int command = atoi(argv[1]);

    shared_memory::set_verbose(false);

    std::string segment = shared_memory_test::segment_id;
    std::string object = shared_memory_test::object_id;

    if (command == shared_memory_test::Actions::set_double)
    {
        shared_memory::set(segment, object, shared_memory_test::test_double);
    }

    if (command == shared_memory_test::Actions::set_int)
    {
        shared_memory::set(segment, object, shared_memory_test::test_int);
    }

    if (command == shared_memory_test::Actions::set_float)
    {
        shared_memory::set(segment, object, shared_memory_test::test_float);
    }

    if (command == shared_memory_test::Actions::set_string)
    {
        shared_memory::set(segment, object, shared_memory_test::test_string);
    }

    if (command == shared_memory_test::Actions::set_vector)
    {
        std::vector<double> v;
        for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
        {
            v.push_back(shared_memory_test::test_array[i]);
        }
        shared_memory::set(segment, object, v);
    }

    if (command == shared_memory_test::Actions::set_eigen_vector)
    {
        Eigen::VectorXd v(shared_memory_test::test_array_size);
        for (int i = 0; i < v.size(); i++)
        {
            v(i) = shared_memory_test::test_array[i];
        }
        shared_memory::set(segment, object, v);
    }

    if (command == shared_memory_test::Actions::set_int_double_map)
    {
        std::map<int, double> m;
        m[shared_memory_test::map_int_keys1] = shared_memory_test::map_value_1;
        m[shared_memory_test::map_int_keys2] = shared_memory_test::map_value_2;
        shared_memory::set(segment, object, m);
    }

    if (command == shared_memory_test::Actions::set_string_double_map)
    {
        std::map<std::string, double> m;
        m[shared_memory_test::map_string_keys1] =
            shared_memory_test::map_value_1;
        m[shared_memory_test::map_string_keys2] =
            shared_memory_test::map_value_2;
        shared_memory::set(segment, object, m);
    }

    if (command == shared_memory_test::Actions::set_double_array)
    {
        shared_memory::set(segment,
                           object,
                           shared_memory_test::test_array,
                           shared_memory_test::test_array_size);
    }

    if (command == shared_memory_test::Actions::set_string_vector_double_map)
    {
        std::map<std::string, std::vector<double>> m;
        std::vector<double> v1(2);
        v1[0] = shared_memory_test::map_value_1;
        v1[1] = shared_memory_test::map_value_2;
        std::vector<double> v2(2);
        v2[0] = shared_memory_test::map_value_2;
        v2[1] = shared_memory_test::map_value_1;
        m[shared_memory_test::map_string_keys1] = v1;
        m[shared_memory_test::map_string_keys2] = v2;
        shared_memory::set(segment, object, m);
    }

    if (command == shared_memory_test::Actions::set_string_vector_eigen_map)
    {
        std::map<std::string, Eigen::VectorXd> m;
        Eigen::VectorXd v1(2);
        v1[0] = shared_memory_test::map_value_1;
        v1[1] = shared_memory_test::map_value_2;
        Eigen::VectorXd v2(2);
        v2[0] = shared_memory_test::map_value_2;
        v2[1] = shared_memory_test::map_value_1;
        m[shared_memory_test::map_string_keys1] = v1;
        m[shared_memory_test::map_string_keys2] = v2;
        shared_memory::set(segment, object, m);
    }

    if (command == shared_memory_test::Actions::concurrent_1)
    {
        std::stringstream cond_var_name;
        cond_var_name << shared_memory_test::segment_id << "_"
                      << shared_memory_test::concurrent_1;
        shared_memory::LockedConditionVariable cond_var(cond_var_name.str());

        shared_memory::set(shared_memory_test::segment_id,
                           shared_memory_test::concurrent_proc1_ready,
                           true);
        cond_var.lock_scope();
        cond_var.wait();

        // Send shared_memory_test::concurrent_value_1
        double d[shared_memory_test::test_array_size];
        for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
        {
            d[i] = shared_memory_test::concurrent_value_1;
        }
        for (int i = 0; i < 5000; i++)
        {
            shared_memory::set(
                segment, object, d, shared_memory_test::test_array_size);
            if (i == 0)
            {
                cond_var.unlock_scope();
                cond_var.notify_all();
            }
            usleep(50);
        }
        // Send stop
        for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
        {
            d[i] = shared_memory_test::concurrent_stop_value;
        }
        shared_memory::set(
            segment, object, d, shared_memory_test::test_array_size);
    }

    if (command == shared_memory_test::Actions::concurrent_2)
    {
        std::stringstream cond_var_name;
        cond_var_name << shared_memory_test::segment_id << "_"
                      << shared_memory_test::concurrent_2;
        shared_memory::LockedConditionVariable cond_var(cond_var_name.str());
        shared_memory::set(shared_memory_test::segment_id,
                           shared_memory_test::concurrent_proc2_ready,
                           true);
        cond_var.lock_scope();
        cond_var.wait();

        double d[shared_memory_test::test_array_size];
        for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
        {
            d[i] = shared_memory_test::concurrent_value_2;
        }
        for (int i = 0; i < 5000; i++)
        {
            shared_memory::set(
                segment, object, d, shared_memory_test::test_array_size);
            if (i == 0)
            {
                cond_var.unlock_scope();
                cond_var.notify_all();
            }
            usleep(50);
        }
        for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
        {
            d[i] = shared_memory_test::concurrent_stop_value;
        }
        shared_memory::set(
            segment, object, d, shared_memory_test::test_array_size);
    }

    if (command == shared_memory_test::Actions::locked_condition_variable)
    {
        // create a data vector
        double d[shared_memory_test::test_array_size];

        std::cout << "TESTS_EXECUTABLE: Get the cond var." << std::endl;
        shared_memory::LockedConditionVariable cond_var(
            shared_memory_test::segment_id);

        // from here all variables are protected
        std::cout << "TESTS_EXECUTABLE: Lock scope." << std::endl;
        cond_var.lock_scope();

        std::cout << "TESTS_EXECUTABLE: Set ShM." << std::endl;
        // fill d with a starting value
        for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
        {
            d[i] = shared_memory_test::concurrent_value_2;
        }
        // write d in the shared memory
        shared_memory::set(
            segment, object, d, shared_memory_test::test_array_size);

        // we wait that the value are read
        std::cout << "TESTS_EXECUTABLE: notify." << std::endl;
        cond_var.notify_all();
        std::cout << "TESTS_EXECUTABLE: wait." << std::endl;
        cond_var.wait();

        std::cout << "TESTS_EXECUTABLE: Set ShM." << std::endl;
        for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
        {
            d[i] = shared_memory_test::concurrent_stop_value;
        }
        shared_memory::set(
            segment, object, d, shared_memory_test::test_array_size);

        // we wake the clients and finish
        std::cout << "TESTS_EXECUTABLE: Notify." << std::endl;
        cond_var.notify_all();
        std::cout << "TESTS_EXECUTABLE: Wait." << std::endl;
        cond_var.wait();
        std::cout << "TESTS_EXECUTABLE: Unlock scope." << std::endl;
        cond_var.unlock_scope();
    }

    if (command == shared_memory_test::Actions::condition_variable)
    {
        std::cout << "TESTS_EXECUTABLE: Get mutex and cond var." << std::endl;
        std::string segment_mutex(shared_memory_test::segment_mutex_id);
        shared_memory::Mutex mutex(shared_memory_test::segment_mutex_id, false);
        shared_memory::ConditionVariable condition(
            shared_memory_test::segment_cv_id, false);

        double v[shared_memory_test::test_array_size];
        int value = 2;
        for (int i = 0; i < 10; i++)
        {
            std::cout << "TESTS_EXECUTABLE: Lock." << std::endl;
            shared_memory::Lock lock(mutex);

            std::cout << "TESTS_EXECUTABLE: Set ShM." << std::endl;
            for (unsigned int i = 0; i < shared_memory_test::test_array_size;
                 i++)
            {
                v[i] = value;
            }
            shared_memory::set(shared_memory_test::segment_id,
                               shared_memory_test::object_id,
                               v,
                               shared_memory_test::test_array_size);
            std::cout << "TESTS_EXECUTABLE: Notify." << std::endl;
            condition.notify_one();
            std::cout << "TESTS_EXECUTABLE: Wait." << std::endl;
            condition.wait(lock);
        }
        condition.notify_one();
    }

    if (command == shared_memory_test::Actions::exchange_manager)
    {
        bool leading = false;
        bool autolock = true;

        shared_memory::Exchange_manager_consumer<shared_memory::Four_int_values,
                                                 DATA_EXCHANGE_QUEUE_SIZE>
            consumer(segment, object, leading, autolock);

        int nb_consumed = 0;
        shared_memory::Four_int_values fiv;
        int max_wait = 1000000;  // 1 second
        int waited = 0;

        while (nb_consumed < shared_memory_test::nb_to_consume)
        {
            if (consumer.ready_to_consume())
            {
                bool received = consumer.consume(fiv);
                if (received)
                {
                    waited = 0;
                    nb_consumed += 1;
                }
                else
                {
                    usleep(100);
                    waited += 100;
                }
            }
            else
            {
                usleep(100);
                waited += 100;
            }
            if (waited >= max_wait)
            {
                // waiting for too long, exiting
                shared_memory::set(
                    shared_memory_test::exchange_manager_segment_id,
                    shared_memory_test::exchange_manager_object_id,
                    true);
                break;
            }

            // letting a chance for the producer to get the lock
            usleep(10);
        }

        // making sure the producer receives all the feedbacks
        waited = 0;
        while (!consumer.purge_feedbacks())
        {
            usleep(100);
            waited += 100;
            if (waited >= max_wait)
            {
                // waiting for too long, exiting
                shared_memory::set(
                    shared_memory_test::exchange_manager_segment_id,
                    shared_memory_test::exchange_manager_object_id,
                    true);
                break;
            }

            // letting a chance for the producer to get the lock
            usleep(10);
        }
    }

    return 0;
}
