/**
 * @file unit_tests.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Shared memroy API unittests.
 */

#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include "gtest/gtest.h"
#include "shared_memory/array.hpp"
#include "shared_memory/condition_variable.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include "shared_memory/demos/item.hpp"
#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/exchange_manager_producer.hpp"
#include "shared_memory/lock.hpp"
#include "shared_memory/locked_condition_variable.hpp"
#include "shared_memory/mutex.hpp"
#include "shared_memory/shared_memory.hpp"
#include "shared_memory/tests/tests.h"

// set in the CMakeLists.txt file
// see tests/support/tests_executable.cpp
static std::string PATH_TO_EXECUTABLE = SHM_PATH_TO_SUPPORT_EXE;

static unsigned int TIME_SLEEP = 20000;  // microseconds

static inline void clear_memory()
{
    shared_memory::clear_shared_memory(shared_memory_test::segment_id);
}

class SharedMemoryTests : public ::testing::Test
{
protected:
    void SetUp()
    {
        clear_memory();
        shared_memory::set_verbose(false);
        shared_memory::get_segment_mutex(shared_memory_test::segment_id)
            .unlock();
    }
    void TearDown()
    {
        clear_memory();
    }
};

class DISABLED_SharedMemoryTests : public ::testing::Test
{
};

static bool _call_executable(int action, bool blocking = false);

static bool _call_executable(int action, bool blocking)
{
    std::stringstream s;
    s << PATH_TO_EXECUTABLE << " " << action;
    if (!blocking)
    {
        s << " &";
    }
    s << std::ends;
    if (blocking)
    {
        if (0 != std::system(s.str().c_str()))
        {
            return false;
        }
        return true;
    }
    std::system(s.str().c_str());
    return true;
}

TEST_F(SharedMemoryTests, spawn_thread_test)
{
    bool called = _call_executable(shared_memory_test::set_double, true);
    ASSERT_EQ(called, true);
}

TEST_F(SharedMemoryTests, double_test)
{
    _call_executable(shared_memory_test::set_double, true);

    double value;
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, value);
    ASSERT_EQ(value, shared_memory_test::test_double);
}

TEST_F(SharedMemoryTests, int_test)
{
    _call_executable(shared_memory_test::set_int, true);

    int value;
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, value);
    ASSERT_EQ(value, shared_memory_test::test_int);
}

TEST_F(SharedMemoryTests, test_float)
{
    _call_executable(shared_memory_test::set_float, true);

    float value;
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, value);
    ASSERT_EQ(value, shared_memory_test::test_float);
}

TEST_F(SharedMemoryTests, test_string)
{
    _call_executable(shared_memory_test::set_string, true);

    std::string str;
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, str);
    ASSERT_EQ(str, shared_memory_test::test_string);
}

TEST_F(SharedMemoryTests, test_array)
{
    _call_executable(shared_memory_test::set_double_array, true);

    double a[shared_memory_test::test_array_size];
    shared_memory::get(shared_memory_test::segment_id,
                       shared_memory_test::object_id,
                       a,
                       shared_memory_test::test_array_size);

    for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
    {
        ASSERT_EQ(a[i], shared_memory_test::test_array[i]);
    }
}

TEST_F(SharedMemoryTests, test_vector)
{
    _call_executable(shared_memory_test::set_vector, true);

    std::vector<double> v(shared_memory_test::test_array_size);
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, v);

    ASSERT_EQ(v.size(), shared_memory_test::test_array_size);
    for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
    {
        ASSERT_EQ(v[i], shared_memory_test::test_array[i]);
    }
}

TEST_F(SharedMemoryTests, test_eigen_vector)
{
    _call_executable(shared_memory_test::set_vector, true);

    Eigen::VectorXd v(shared_memory_test::test_array_size);
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, v);

    ASSERT_EQ(v.size(), shared_memory_test::test_array_size);
    for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
    {
        ASSERT_EQ(v(i), shared_memory_test::test_array[i]);
    }
}

TEST_F(SharedMemoryTests, test_int_double_map)
{
    _call_executable(shared_memory_test::set_int_double_map, true);

    std::map<int, double> m;
    m[shared_memory_test::map_int_keys1] = 0.0;
    m[shared_memory_test::map_int_keys2] = 0.0;
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, m);

    ASSERT_EQ(m.size(), shared_memory_test::test_map_size);
    ASSERT_EQ(m[shared_memory_test::map_int_keys1],
              shared_memory_test::map_value_1);
    ASSERT_EQ(m[shared_memory_test::map_int_keys2],
              shared_memory_test::map_value_2);
}

TEST_F(SharedMemoryTests, test_string_double_map)
{
    _call_executable(shared_memory_test::set_string_double_map, true);

    std::map<std::string, double> m;
    m[shared_memory_test::map_string_keys1] = 0.0;
    m[shared_memory_test::map_string_keys2] = 0.0;
    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, m);

    ASSERT_EQ(m.size(), shared_memory_test::test_map_size);
    ASSERT_EQ(m[shared_memory_test::map_string_keys1],
              shared_memory_test::map_value_1);
    ASSERT_EQ(m[shared_memory_test::map_string_keys2],
              shared_memory_test::map_value_2);
}

TEST_F(SharedMemoryTests, test_string_vector_double_map)
{
    _call_executable(shared_memory_test::set_string_vector_double_map, true);

    std::map<std::string, std::vector<double>> m;

    std::vector<double> v1(2);
    v1[0] = 0.0;
    v1[1] = 0.0;

    std::vector<double> v2(2);
    v2[0] = 0.0;
    v2[1] = 0.0;

    m[shared_memory_test::map_string_keys1] = v1;
    m[shared_memory_test::map_string_keys2] = v2;

    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, m);

    ASSERT_EQ(m.size(), shared_memory_test::test_map_size);
    ASSERT_EQ(m[shared_memory_test::map_string_keys1].size(),
              static_cast<unsigned int>(2));
    ASSERT_EQ(m[shared_memory_test::map_string_keys2].size(),
              static_cast<unsigned int>(2));

    ASSERT_EQ(m[shared_memory_test::map_string_keys1][0],
              shared_memory_test::map_value_1);
    ASSERT_EQ(m[shared_memory_test::map_string_keys1][1],
              shared_memory_test::map_value_2);

    ASSERT_EQ(m[shared_memory_test::map_string_keys2][0],
              shared_memory_test::map_value_2);
    ASSERT_EQ(m[shared_memory_test::map_string_keys2][1],
              shared_memory_test::map_value_1);
}

TEST_F(SharedMemoryTests, test_string_vector_eigen_map)
{
    _call_executable(shared_memory_test::set_string_vector_eigen_map, true);

    std::map<std::string, Eigen::VectorXd> m;

    Eigen::VectorXd v1(2), v2(2);

    m[shared_memory_test::map_string_keys1] = v1;
    m[shared_memory_test::map_string_keys2] = v2;

    shared_memory::get(
        shared_memory_test::segment_id, shared_memory_test::object_id, m);

    ASSERT_EQ(m.size(), shared_memory_test::test_map_size);
    ASSERT_EQ(m[shared_memory_test::map_string_keys1].size(), 2);
    ASSERT_EQ(m[shared_memory_test::map_string_keys2].size(), 2);

    ASSERT_EQ(m[shared_memory_test::map_string_keys1][0],
              shared_memory_test::map_value_1);
    ASSERT_EQ(m[shared_memory_test::map_string_keys1][1],
              shared_memory_test::map_value_2);

    ASSERT_EQ(m[shared_memory_test::map_string_keys2][0],
              shared_memory_test::map_value_2);
    ASSERT_EQ(m[shared_memory_test::map_string_keys2][1],
              shared_memory_test::map_value_1);
}

TEST_F(SharedMemoryTests, test_memory_overflow)
{
    unsigned int max_size = DEFAULT_SHARED_MEMORY_SIZE / sizeof(int) + 1;
    std::vector<int> v(max_size);
    for (size_t i = 0; i < v.size(); i++) v[i] = 1;

    ASSERT_THROW(
        shared_memory::set(
            shared_memory_test::segment_id, shared_memory_test::object_id, v),
        shared_memory::Allocation_exception);
}

TEST_F(SharedMemoryTests, test_wrong_size_vector)
{
    _call_executable(shared_memory_test::set_vector, true);

    std::vector<double> v(shared_memory_test::test_array_size + 1);  // !

    ASSERT_THROW(
        shared_memory::get(
            shared_memory_test::segment_id, shared_memory_test::object_id, v),
        shared_memory::Unexpected_size_exception);
}

static inline bool is_one_of(double v, double a1, double a2)
{
    if (v == a1) return true;
    if (v == a2) return true;
    return false;
}

TEST_F(SharedMemoryTests, test_concurrency)
{
    // condition variables' names
    std::stringstream cond_var_name1;
    cond_var_name1 << shared_memory_test::segment_id << "_"
                   << shared_memory_test::concurrent_1;
    std::stringstream cond_var_name2;
    cond_var_name2 << shared_memory_test::segment_id << "_"
                   << shared_memory_test::concurrent_2;
    // destroy the condition variables
    {
        shared_memory::LockedConditionVariable cond_var1(cond_var_name1.str(),
                                                         true);
        shared_memory::LockedConditionVariable cond_var2(cond_var_name2.str(),
                                                         true);
    }

    _call_executable(shared_memory_test::concurrent_1);
    _call_executable(shared_memory_test::concurrent_2);

    usleep(TIME_SLEEP);

    // Prepare the condition variables
    shared_memory::LockedConditionVariable cond_var1(cond_var_name1.str());
    cond_var1.lock_scope();
    //
    shared_memory::LockedConditionVariable cond_var2(cond_var_name2.str());
    cond_var2.lock_scope();

    bool set_1_observed = false;
    bool set_2_observed = false;

    int nb_1 = 0;
    int nb_2 = 0;

    double a[shared_memory_test::test_array_size];

    cond_var1.notify_all();
    cond_var2.notify_all();
    cond_var1.wait();
    cond_var2.wait();
    cond_var1.unlock_scope();
    cond_var2.unlock_scope();

    while (true)
    {
        shared_memory::get(shared_memory_test::segment_id,
                           shared_memory_test::object_id,
                           a,
                           shared_memory_test::test_array_size);

        if (a[0] == shared_memory_test::concurrent_stop_value)
        {
            break;  // the loop stops here.
        }

        if (a[0] == shared_memory_test::concurrent_value_1)
        {
            set_1_observed = true;
            nb_1++;
        }

        if (a[0] == shared_memory_test::concurrent_value_2)
        {
            set_2_observed = true;
            nb_2++;
        }

        ASSERT_EQ(is_one_of(a[0],
                            shared_memory_test::concurrent_value_1,
                            shared_memory_test::concurrent_value_2),
                  true);

        for (unsigned int i = 1; i < shared_memory_test::test_array_size; i++)
        {
            ASSERT_EQ(a[0], a[i]);
        }

        usleep(5);
    }
    ASSERT_EQ(set_1_observed, true);
    ASSERT_EQ(set_2_observed, true);
}

TEST_F(SharedMemoryTests, test_locked_condition_variable)
{
    // create a data vector
    double d[shared_memory_test::test_array_size];

    // get a condition variable
    shared_memory::LockedConditionVariable cond_var(
        shared_memory_test::segment_id);

    _call_executable(shared_memory_test::locked_condition_variable);

    cond_var.lock_scope();
    cond_var.wait();

    shared_memory::get(shared_memory_test::segment_id,
                       shared_memory_test::object_id,
                       d,
                       shared_memory_test::test_array_size);

    ASSERT_EQ(d[0], shared_memory_test::concurrent_value_2);
    for (unsigned int i = 1; i < shared_memory_test::test_array_size; i++)
    {
        ASSERT_EQ(d[0], d[i]);
    }

    cond_var.notify_all();
    cond_var.wait();

    shared_memory::get(shared_memory_test::segment_id,
                       shared_memory_test::object_id,
                       d,
                       shared_memory_test::test_array_size);

    ASSERT_EQ(d[0], shared_memory_test::concurrent_stop_value);
    for (unsigned int i = 1; i < shared_memory_test::test_array_size; i++)
    {
        ASSERT_EQ(d[0], d[i]);
    }

    cond_var.notify_all();
    cond_var.unlock_scope();
    usleep(TIME_SLEEP);
}

TEST_F(SharedMemoryTests, test_timed_wait)
{
    // get a condition variable
    shared_memory::LockedConditionVariable cond_var(
        shared_memory_test::segment_id);
    cond_var.lock_scope();
    ASSERT_FALSE(cond_var.timed_wait(10));
    cond_var.unlock_scope();
}

TEST_F(SharedMemoryTests, test_condition_variable)
{
    // initializing shared array
    int value = 1;
    double v[shared_memory_test::test_array_size];
    for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
    {
        v[i] = value;
    }
    shared_memory::set(shared_memory_test::segment_id,
                       shared_memory_test::object_id,
                       v,
                       shared_memory_test::test_array_size);

    // in case previous run crashed without cleaning up
    std::string segment_mutex(shared_memory_test::segment_mutex_id);
    shared_memory::Mutex::clean(shared_memory_test::segment_mutex_id);

    // get a condition variable
    shared_memory::Mutex mutex(segment_mutex, true);
    shared_memory::ConditionVariable condition(
        shared_memory_test::segment_cv_id, true);

    // starting another process with same condition variable
    _call_executable(shared_memory_test::condition_variable);
    usleep(3 * TIME_SLEEP);

    // other process should be hanging, freeing it
    condition.notify_one();

    for (int i = 0; i < 10; i++)
    {
        {
            shared_memory::Lock lock(mutex);
            condition.wait(lock);

            for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
            {
                v[i] = value;
            }

            shared_memory::set(shared_memory_test::segment_id,
                               shared_memory_test::object_id,
                               v,
                               shared_memory_test::test_array_size);

            usleep(500);

            shared_memory::get(shared_memory_test::segment_id,
                               shared_memory_test::object_id,
                               v,
                               shared_memory_test::test_array_size);

            for (unsigned int i = 0; i < shared_memory_test::test_array_size; i++)
            {
                ASSERT_EQ(v[i], value);
            }
        }

        condition.notify_one();
    }

    condition.notify_one();
    // Wait to make sure the executable is done
    usleep(3 * TIME_SLEEP);
}

TEST_F(SharedMemoryTests, exchange_manager)
{
    bool leading = true;
    bool autolock = true;  // we will not need to call producer.lock()

    typedef shared_memory::Exchange_manager_producer<
        shared_memory::Four_int_values,
        DATA_EXCHANGE_QUEUE_SIZE>
        Producer;

    Producer::clean_mutex(shared_memory_test::segment_id);
    Producer::clean_memory(shared_memory_test::segment_id);

    Producer producer(shared_memory_test::segment_id,
                      shared_memory_test::object_id,
                      leading,
                      autolock);

    // several iterations to make sure the producer can manage 2 consumers
    // running in a row

    for (int iteration = 0; iteration < 10; iteration++)
    {
        producer.reset_char_count();

        // for informing main process if failure occured
        shared_memory::clear_shared_memory(
            shared_memory_test::exchange_manager_segment_id);
        shared_memory::delete_segment(
            shared_memory_test::exchange_manager_segment_id);
        shared_memory::set<bool>(
            shared_memory_test::exchange_manager_segment_id,
            shared_memory_test::exchange_manager_object_id,
            false);

        _call_executable(shared_memory_test::exchange_manager);

        int max_wait = 1000000;  // 1 seconds
        int waited = 0;
        bool failed_to_start = false;
        while (!producer.ready_to_produce())
        {
            usleep(100);
            waited += 100;
            if (waited > max_wait)
            {
                failed_to_start = true;
            }
        }
        ASSERT_EQ(failed_to_start, false);

        // producing items

        int id = 0;
        waited = 0;
        bool failed_to_produce_all = false;
        std::deque<int> consumed;
        int consumed_id = 0;

        int written = 0;
        int previous_written = -1;

        while (id < shared_memory_test::nb_to_consume ||
               consumed_id < shared_memory_test::nb_to_consume)
        {
            // producing items
            if (id < shared_memory_test::nb_to_consume)
            {
                try
                {
                    shared_memory::Four_int_values p(1, 1, 1, 1);
                    p.set_id(id);
                    producer.set(p);
                    id++;
                    waited = 0;
                }
                catch (shared_memory::Memory_overflow_exception)
                {
                    usleep(200);
                    waited += 200;
                    if (waited >= max_wait)
                    {
                        failed_to_produce_all = true;
                        break;
                    }
                }
            }

            // monitoring consumed items
            // should have been consumed in same order
            // as produced
            producer.get(consumed);
            while (!consumed.empty())
            {
                ASSERT_EQ(consumed_id, consumed.front());
                consumed_id++;
                consumed.pop_front();
            }

            // everything supposed to be produced was produced,
            // and consumer died, so exiting
            if (id >= shared_memory_test::nb_to_consume &&
                !producer.ready_to_produce())
            {
                break;
            }

            written = producer.nb_char_written();
            if (written != previous_written)
            {
                previous_written = written;
            }

            // letting a chance for the consumer to get the lock
            usleep(10);
        }

        ASSERT_EQ(failed_to_produce_all, false);

        bool command_failed;
        shared_memory::get<bool>(
            shared_memory_test::exchange_manager_segment_id,
            shared_memory_test::exchange_manager_object_id,
            command_failed);
        ASSERT_EQ(command_failed, false);
    }
}

TEST_F(SharedMemoryTests, exchange_manager_init)
{
    bool leading = true;
    bool autolock = true;  // we will not need to call producer.lock()

    typedef shared_memory::Exchange_manager_producer<
        shared_memory::Four_int_values,
        DATA_EXCHANGE_QUEUE_SIZE>
        Producer;

    typedef shared_memory::Exchange_manager_consumer<
        shared_memory::Four_int_values,
        DATA_EXCHANGE_QUEUE_SIZE>
        Consumer;

    Producer::clean_mutex(shared_memory_test::segment_id);
    Producer::clean_memory(shared_memory_test::segment_id);

    Producer producer(shared_memory_test::segment_id,
                      shared_memory_test::object_id,
                      leading,
                      autolock);

    Consumer consumer(shared_memory_test::segment_id,
                      shared_memory_test::object_id,
                      !leading);

    bool consumed;
    if (consumer.ready_to_consume())
    {
        shared_memory::Four_int_values fiv;
        consumed = consumer.consume(fiv);
    }

    ASSERT_EQ(consumed, false);
}

TEST_F(SharedMemoryTests, serialization)
{
    shared_memory::clear_shared_memory("test_ser");

    shared_memory::Four_int_values in1(1, 1, 2, 1);
    shared_memory::Four_int_values in2(1, 2, 1, 1);

    shared_memory::Four_int_values out1;
    shared_memory::Four_int_values out2;

    shared_memory::serialize("test_ser", "obj1", in1);
    shared_memory::serialize("test_ser", "obj2", in2);

    shared_memory::deserialize("test_ser", "obj1", out1);
    shared_memory::deserialize("test_ser", "obj2", out2);

    ASSERT_EQ(in1.same(out1), true);
    ASSERT_EQ(in2.same(out2), true);
}

TEST_F(SharedMemoryTests, serialization2)
{
    shared_memory::clear_shared_memory("test_ser");

    shared_memory::Serializer<shared_memory::Item<10>> serializer;

    for (int v = 1; v < 100; v++)
    {
        shared_memory::Item<10> in(v);
        shared_memory::Item<10> out;
        const std::string& s = serializer.serialize(in);
        serializer.deserialize(s, out);
        ASSERT_EQ(in.get(), out.get());
    }
}

static shared_memory::array<int> get_array_int()
{
    shared_memory::array<int> a("test_array", 10, true, true);
    int value = 5;
    a.set(2, value);
    return a;
}

TEST_F(SharedMemoryTests, array_int)
{
    shared_memory::clear_array("test_array");

    shared_memory::array<int> a("test_array", 10, true, true);
    int value = 5;
    a.set(2, value);
    a.get(2, value);
    ASSERT_EQ(value, 5);

    shared_memory::array<int> b(a);
    b.get(2, value);
    ASSERT_EQ(value, 5);

    shared_memory::array<int> c = std::move(a);
    c.get(2, value);
    ASSERT_EQ(value, 5);

    shared_memory::array<int> d = get_array_int();
    d.get(2, value);
    ASSERT_EQ(value, 5);
}

TEST_F(SharedMemoryTests, array_array_int)
{
    shared_memory::clear_array("test_array");

    int size = 100;

    shared_memory::array<int, 10> a("test_array", size, true, true);

    int values0[10];
    int values1[10];
    for (uint j = 0; j < 10; j++)
    {
        values0[j] = 2;
        values1[j] = 3;
    }
    a.set(0, *values0);
    a.set(1, *values1);

    a.get(0, *values1);
    a.get(1, *values0);

    for (uint j = 0; j < 10; j++)
    {
        ASSERT_EQ(values0[j], 3);
        ASSERT_EQ(values1[j], 2);
    }

    int values[10];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            values[j] = (i + j);
        }
        a.set(i, *values);
    }

    shared_memory::array<int, 10> b(a);

    for (int i = 0; i < size; i++)
    {
        a.get(i, *values);
        for (int j = 0; j < 10; j++)
        {
            ASSERT_EQ(values[j], i + j);
        }
        b.get(i, *values);
        for (int j = 0; j < 10; j++)
        {
            ASSERT_EQ(values[j], i + j);
        }
    }
}

TEST_F(SharedMemoryTests, array_serializable)
{
    shared_memory::clear_array("test_array");

    int size = 100;

    shared_memory::array<shared_memory::Item<10>> a(
        "test_array", size, true, true);

    for (int i = 0; i < size; i++)
    {
        shared_memory::Item<10> item(i);
        a.set(i, item);
    }

    shared_memory::Item<10> item;
    for (int i = 0; i < size; i++)
    {
        a.get(i, item);
        ASSERT_EQ(item.get(), i);
    }

    shared_memory::array<shared_memory::Item<10>> b(a);
    for (int i = 0; i < size; i++)
    {
        b.get(i, item);
        ASSERT_EQ(item.get(), i);
    }
}

TEST_F(SharedMemoryTests, segment_memory_size)
{
    shared_memory::clear_array("ut_sg_mem_size");

    bool error = false;
    for (int i = 1; i < 10; i++)
    {
        try
        {
            shared_memory::set_segment_sizes(i);
            shared_memory::clear_array("ut_sg_mem_size");
        }
        catch (...)
        {
            error = true;
        }
        ASSERT_EQ(error, false);
    }
}

TEST_F(SharedMemoryTests, segment_info)
{
    shared_memory::clear_shared_memory("test_info");

    shared_memory::set_default_segment_sizes();

    shared_memory::set<double>("test_info", "d1", 5.0);
    shared_memory::set<double>("test_info", "d2", 10.0);

    shared_memory::SegmentInfo si =
        shared_memory::get_segment_info("test_info");
    uint size = si.get_size();
    uint free = si.get_free_memory();
    bool issues = si.has_issues();
    uint nb_objects = si.nb_objects();

    ASSERT_EQ(size, uint(DEFAULT_SHARED_MEMORY_SIZE));
    ASSERT_GT(size, free);
    ASSERT_GT(free, uint(0));
    ASSERT_EQ(nb_objects, uint(2));
    ASSERT_FALSE(issues);

    shared_memory::set<double>("test_info", "d3", 2.0);
    shared_memory::set<double>("test_info", "d4", 3.0);

    shared_memory::SegmentInfo si2 =
        shared_memory::get_segment_info("test_info");
    uint size2 = si2.get_size();
    uint free2 = si2.get_free_memory();
    bool issues2 = si2.has_issues();
    uint nb_objects2 = si2.nb_objects();

    ASSERT_EQ(size, size2);
    ASSERT_LT(free2, free);
    ASSERT_FALSE(issues2);
    ASSERT_EQ(nb_objects2, uint(4));

    // memory overflow on purpose
    std::vector<char> v(free2 + 1);
    bool exception = false;
    try
    {
        shared_memory::set("test_info", "unreasonable", v);
    }
    catch (...)
    {
        exception = true;
    }
    ASSERT_TRUE(exception);
}
