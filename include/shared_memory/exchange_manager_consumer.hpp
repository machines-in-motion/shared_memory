#ifndef EXCHANGE_MANAGER_CONSUMER_HPP
#define EXCHANGE_MANAGER_CONSUMER_HPP

#include <string>
#include <iostream>
#include <boost/lockfree/queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/thread_synchronisation.hpp"

namespace bip = boost::interprocess;

namespace shared_memory {


  template<class Serializable, int QUEUE_SIZE>
  class Exchange_manager_consumer {

    
    typedef boost::lockfree::queue<int,
				   boost::lockfree::fixed_sized<true>,
				   boost::lockfree::capacity<Serializable::serialization_size*QUEUE_SIZE>> producer_queue;
    typedef boost::lockfree::queue<int,
				   boost::lockfree::fixed_sized<true>,
				   boost::lockfree::capacity<Serializable::serialization_size*QUEUE_SIZE>> consumer_queue;

    
  public:

    Exchange_manager_consumer(std::string segment_id,
			      std::string object_id,
			      bool autolock=true,
			      bool clean_memory_on_destruction=false);


    ~Exchange_manager_consumer();

    void lock();
    void unlock();

    bool consume(Serializable &serializable,bool no_lock=true);

    void purge();
    
  public:

    static void clean_mutex(std::string segment_id);
    static void clean_memory(std::string segment_id);
    

  private:

    std::string segment_id_;
    std::string object_id_producer_;
    std::string object_id_consumer_;
    bip::managed_shared_memory segment_;
    producer_queue *produced_;
    consumer_queue *consumed_;
    double *values_;
    int previous_consumed_id_;
    bool clean_memory_;

    // consumes lock the condition variable automatically.
    // if false: the lock function has to be called
    bool autolock_;

    // to synchronize with producer
    shared_memory::Mutex locker_;
    
  };


  #include "exchange_manager_consumer.hxx"


}




#endif // EXCHANGE_MANAGER_CONSUMER_HPP
