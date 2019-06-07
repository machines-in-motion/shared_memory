#ifndef EXCHANGE_MANAGER_CONSUMER_HPP
#define EXCHANGE_MANAGER_CONSUMER_HPP

#include "shared_memory/exchange_manager_memory.hpp"


namespace bip = boost::interprocess;

namespace shared_memory {


  template<class Serializable, int QUEUE_SIZE>
  class Exchange_manager_consumer {

  typedef Exchange_manager_memory<Serializable,QUEUE_SIZE > Memory;
  typedef std::shared_ptr< Memory >  Memory_ptr;
    
    
  public:

    /**
     * @brief An exchange_manager_consumer reads from the shared memory
     * serialized items produced by an instance of 
     * exchange_manager_producer (which should use the same
     * segment_id and object_id), possibly running in a separate process.
     * @param segment_id id of the shared memory segment
     * @param object_id id of the shared memory object prefix
     * @param the consumer is to be "permanent", while different consumers
     * may consume its data. Implies the deletion of the underlying share memory
     * upon destruction.
     * @param mutex locking mechanism internally managed by the 
     * producer. If false, lock has to be "manually" called. This allows for
     * example to set several items in one shot
     */
    Exchange_manager_consumer( std::string segment_id,
			       std::string object_id,
			       bool leading,
			       bool autolock=true );


    ~Exchange_manager_consumer();

    /** @brief lock the mutex required for writting in the shared memory
     *  without any collision with any producer. Should be called 
     *  before calls to "consume". Not required if
     *  the constructor was called with autolock set to true
     */
    void lock();

    /** @brief unlock the mutex for writting in the shared memory
     *  without any collision with any producer. Not required if
     *  the constructor was called with autolock set to true
     */
    void unlock();

    /** @brief read from the underlying shared memory 
     *  a serialized object (set by a producer). 
     *  Should be called only if ready_to_consume returns true.
     *  @return true if an item has been read
     */
    bool consume(Serializable &serializable);

    /** @brief returns true if a producer is also running.
     *  'consume' should be called only if ready_to_consume returns true. 
     */
    bool ready_to_consume();
    
  public:

    /* @brief Clean the underlying shared memory. 
     * Call should not be required if the constructor has 
     * been called with "clean_memory_on_exit" to true. Yet, maybe useful if the program 
     * crashed without calling the destructor as it should have had.
     */
    static void clean_mutex(std::string segment_id);

    /* @brief Unlock and delete the underlying shared memory mutex. 
     * Call should not be required if the constructor has 
     * been called with "clean_memory_on_exit" to true. Yet, maybe useful if the program 
     * crashed without calling the destructor as it should have had.
     */
    static void clean_memory(std::string segment_id);

  private:

    void reset();
    

  private:

    // will do all shared memory operations
    Memory_ptr memory_;
    
    // will clean the memory at destruction
    // and each time a producer dies
    bool leading_;
    
    // consumes lock the condition variable automatically.
    // if false: the lock function has to be called
    bool autolock_;

    std::string segment_id_;
    std::string object_id_;
    
  };


  #include "exchange_manager_consumer.hxx"


}




#endif // EXCHANGE_MANAGER_CONSUMER_HPP
