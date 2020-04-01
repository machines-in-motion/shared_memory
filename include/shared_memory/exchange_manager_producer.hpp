/**
 * @file exchange_manager_producer.hpp
 * @author Vincent Berenz (vberenz@tuebingen.mpg.de)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-06-07
 *
 * @brief Interprocess exchange of serialized items
 */

#ifndef EXCHANGE_MANAGER_PRODUCER_HPP
#define EXCHANGE_MANAGER_PRODUCER_HPP

#include "shared_memory/internal/exchange_manager_memory.hpp"

namespace bip = boost::interprocess;

using namespace shared_memory::internal;

namespace shared_memory
{
template <class Serializable, int QUEUE_SIZE>
class Exchange_manager_producer
{
    typedef Exchange_manager_memory<Serializable, QUEUE_SIZE> Memory;
    typedef std::shared_ptr<Memory> Memory_ptr;

public:
    /**
     * @brief An exchange_manager_producer writes in the shared memory
     * serialized items expected to be consumed by an instance of
     * exchange_manager_consumer (which should use the same
     * segment_id and object_id), possibly running in a separate process.
     * @param segment_id id of the shared memory segment
     * @param object_id id of the shared memory object prefix
     * @param autolock: mutex locking mechanism internally managed by the
     * producer. If false, lock has to be "manually" called. This allows for
     * example to set several items in one shot
     * @param clean_memory_on_exit. If true, the destructor will clean
     * the underlined shared memory items.
     */
    Exchange_manager_producer(std::string segment_id,
                              std::string object_id,
                              bool leading,
                              bool autolock = true);

    ~Exchange_manager_producer();

    /** @brief returns true if a consumer is also running.
     *  'set' should be called only if ready_to_produce returns true.
     */
    bool ready_to_produce();

    /** @brief lock the mutex required for writting in the shared memory
     *  without any collision with any consumer. Should be called
     *  before calls to "set". Not required if
     *  the constructor was called with autolock set to true
     */
    void lock();

    /** @brief unlock the mutex for writting in the shared memory
     *  without any collision with any consumer. Not required if
     *  the constructor was called with autolock set to true
     */
    void unlock();

    /** @brief Set this serializable to be consumed. Throws
     * shared_memory::Memory_overflow_exception if the shared memory is full.
     * Some of the shared memory should get free once items have been consumed
     * by a consumer. This method should be called only if 'ready_to_produce'
     *  returns true;
     *  Returns true if all data could be written in the shared memory, false if
     * some data required to be buffered (any following call to set, if any,
     * will perform a new attempt to write remaining buffer to the shared
     * memory)
     */
    bool set(const Serializable &serializable);

    /**
     * @brief removed all elements from the shared queue
     *
     */
    void clear();

    /** @brief write into get_consumed_ids the ids of serialized items that have
     * been successfully consumed by a consumer
     */
    void get(std::deque<int> &get_consumed_ids);

    /** @brief returns the number of characters
     *  that have been serialized and written to
     *  the exchange queue. For debug purposes.
     */
    int nb_char_written();

    /** @brief reset the count
     *  of characters written to
     * the exchange queue to zero
     */
    void reset_char_count();

    bool producer_queue_empty() const;
    bool consumer_queue_empty() const;

private:
    void reset();

private:
    Memory_ptr memory_;
    bool autolock_;
    bool leading_;
    std::string segment_id_;
    std::string object_id_;

public:
    /** @brief (unlock) and erase the mutex from the shared
     *  memory. To be used if some executable using the
     *  exchange manager crashed without calls to destructors.
     */
    static void clean_mutex(std::string segment_id);

    /** @brief wipe out the corresponding shared
     *  memory. To be used if some executable using the
     *  exchange manager crashed without calls to destructors.
     */
    static void clean_memory(std::string segment_id);
};

#include "exchange_manager_producer.hxx"

}  // namespace shared_memory

#endif  // EXCHANGE_MANAGER_PRODUCER_HPP
