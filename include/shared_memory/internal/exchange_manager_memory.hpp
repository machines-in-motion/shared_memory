/**
 * @file exchange_manager_memory.hpp
 * @author Vincent Berenz (vberenz@tuebingen.mpg.de)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-06-07
 *
 * @brief Interprocess exchange of serialized items
 */

#pragma once

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <string>

#include "shared_memory/mutex.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/shared_memory.hpp"

namespace bip = boost::interprocess;

namespace shared_memory
{
namespace internal
{
template <class Serializable>
class Serialized_read
{
public:
    Serialized_read();
    ~Serialized_read();
    void set(char value);
    bool read(Serializable &serializable);
    int nb_char_read();
    void reset_nb_char_read();

private:
    std::deque<char> buffer_;
    int size_;
    int nb_char_read_;
    int serializable_size_;
    char *values_;
    Serializer<Serializable> serializer_;
};

template <class Serializable>
class Serialized_write
{
public:
    Serialized_write();
    ~Serialized_write();
    bool empty();
    char front();
    void pop();
    void write(const Serializable &serializable, std::size_t expected_size);
    int nb_char_written();
    void reset_nb_char_written();

private:
    std::deque<char> buffer_;
    char *values_;
    int nb_char_written_;
    int serializable_size_;
    Serializer<Serializable> serializer_;
};

/* @brief Used internally by Exchange_manager_consumer
 * and Exchange_manager_producer. Do not use directly.
 */
template <class Serializable, int QUEUE_SIZE>
class Exchange_manager_memory
{
    typedef boost::lockfree::queue<char,
                                   boost::lockfree::fixed_sized<true>,
                                   boost::lockfree::capacity<QUEUE_SIZE> >
        producer_queue;
    typedef boost::lockfree::queue<int,
                                   boost::lockfree::fixed_sized<true>,
                                   boost::lockfree::capacity<QUEUE_SIZE> >
        consumer_queue;

public:
    enum Status
    {
        RESET,
        WAITING,
        RUNNING
    };

public:
    Exchange_manager_memory(std::string segment_id, std::string object_id);
    ~Exchange_manager_memory();

    bool read_serialized(Serializable &serializable);
    bool write_serialized(const Serializable &serializable);
    void write_serialized_id(int id);
    void clear();
    void get_consumed_ids(std::deque<int> &get_ids);

    void set_status(Status status);
    void get_status(Status &status);

    bool purge_feedbacks();

    void clean();

    void lock();
    void unlock();

    bool consumer_queue_empty() const;
    bool producer_queue_empty() const;

    void weird_purge();

public:
    // for monitoring the number of characters written/read
    // from the queue
    void reset_char_count();
    int nb_char_written();
    int nb_char_read();

private:
    int nb_char_read_;
    int nb_char_written_;

public:
    static void clean_mutex(std::string segment_id);
    static void clean_memory(std::string segment_id);

public:
    std::string segment_id_;
    std::string object_id_producer_;
    std::string object_id_consumer_;
    std::string object_id_status_;
    bip::managed_shared_memory segment_;
    producer_queue *produced_;
    std::deque<int> consumed_buffer_;
    consumer_queue *consumed_;
    char *values_;
    shared_memory::Mutex locker_;
    Serialized_read<Serializable> serialized_read_;
    Serialized_write<Serializable> serialized_write_;
    int serializable_size_;
};

#include "exchange_manager_memory.hxx"

}  // namespace internal
}  // namespace shared_memory
