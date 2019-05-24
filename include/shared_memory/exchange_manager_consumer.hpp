#ifndef EXCHANGE_MANAGER_CONSUMER_HPP
#define EXCHANGE_MANAGER_CONSUMER_HPP

#include <string>
#include <iostream>
#include <boost/lockfree/queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>

#include "shared_memory/shared_memory.hpp"

namespace bip = boost::interprocess;

namespace shared_memory {


  template<class Serializable, int QUEUE_SIZE>
  class Exchange_manager_consumer {

    
    typedef boost::lockfree::queue<double,
				 boost::lockfree::fixed_sized<true>,
				 boost::lockfree::capacity<QUEUE_SIZE>> producer_queue;
    typedef boost::lockfree::queue<int,
				   boost::lockfree::fixed_sized<true>,
				   boost::lockfree::capacity<QUEUE_SIZE>> consumer_queue;

    
  public:

    Exchange_manager_consumer(std::string segment_id,
			      std::string object_id,
			      bool clean_memory_on_destruction=false);


    ~Exchange_manager_consumer();

    void clean_memory();
    bool consume(Serializable &serializable);


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
    
  };


  #include "exchange_manager_consumer.hxx"


}




#endif // EXCHANGE_MANAGER_CONSUMER_HPP
