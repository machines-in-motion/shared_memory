#ifndef EXCHANGE_MANAGER_PRODUCER_HPP
#define EXCHANGE_MANAGER_PRODUCER_HPP

#include "shared_memory/exchange_manager_memory.hpp"


namespace bip = boost::interprocess;

namespace shared_memory {

  template<class Serializable, int QUEUE_SIZE>
  class Exchange_manager_producer {

  typedef Exchange_manager_memory<Serializable,QUEUE_SIZE > Memory;
  typedef std::shared_ptr< Memory >  Memory_ptr;

    
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
			      bool autolock=true);


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

    /** Set this serializable to be consumed. Throws shared_memory::Memory_overflow_exception 
     *  if the shared memory is full. Some of the shared memory should get free once items have
     *  been consumed by a consumer. This method should be called only if 'ready_to_produce'
     *  returns true;
     */
    void set(const Serializable &serializable);

    /** write into get_consumed_ids the ids of serialized items that have been 
     *  successfully consumed by a consumer
     */
    void get(std::deque<int> &get_consumed_ids);

  private:

    void reset();

  private:

    Memory_ptr memory_;
    bool autolock_;
    bool leading_;
    std::string segment_id_;
    std::string object_id_;
    
    
  };


  #include "exchange_manager_producer.hxx"


}




#endif // EXCHANGE_MANAGER_PRODUCER_HPP
