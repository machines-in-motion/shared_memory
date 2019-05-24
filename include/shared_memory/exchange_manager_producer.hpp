#ifndef EXCHANGE_MANAGER_PRODUCER_HPP
#define EXCHANGE_MANAGER_PRODUCER_HPP

#include <string>
#include <deque>
#include <queue>
#include <stdexcept>
#include <cstring>
#include <iostream>

#include "shared_memory/exceptions.h"
#include "shared_memory/exchange_manager_consumer.hpp"



namespace bip = boost::interprocess;

namespace shared_memory {

  template<class Serializable, int QUEUE_SIZE>
  class Exchange_manager_producer {

    typedef boost::lockfree::queue<double,
				 boost::lockfree::fixed_sized<true>,
				 boost::lockfree::capacity<QUEUE_SIZE>> producer_queue;
    typedef boost::lockfree::queue<int,
				   boost::lockfree::fixed_sized<true>,
				   boost::lockfree::capacity<QUEUE_SIZE>> consumer_queue;

  public:

    /**
     * @brief An exchange_manager_producer writes in the shared memory
     * serialized items expected to be consumed by an instance of 
     * exchange_manager_consumer (which should use the same
     * segment_id and object_id)
     * @param segment_id id of the shared memory segment
     * @param object_id id of the shared memory object prefix
     * @param set to true if an exception should be thrown if the consumer
     * 
     */
    Exchange_manager_producer(std::string segment_id,
			      std::string object_id);

    ~Exchange_manager_producer();

    void set(const Serializable &serializable);
    void get(std::deque<int> &get_consumed_ids);
    
  private:

    int get_last_consumed();

  private:

    std::string segment_id_;
    std::string object_id_producer_;
    std::string object_id_consumer_;
    bip::managed_shared_memory segment_;

    // shared memory queue in which items to consume are written
    producer_queue *produced_;

    // shared memory queue in which ids of consumed items
    // are written by the consumer
    consumer_queue *consumed_;

    bool consumer_started_;
    double *values_;

  };


  #include "exchange_manager_producer.hxx"


}




#endif // EXCHANGE_MANAGER_PRODUCER_HPP
