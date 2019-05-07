#ifndef EXCHANGE_MANAGER_CONSUMER_HPP
#define EXCHANGE_MANAGER_CONSUMER_HPP

#include <string>
#include <iostream>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>


namespace bip = boost::interprocess;

namespace shared_memory {


  typedef boost::lockfree::spsc_queue< double,
				       boost::lockfree::capacity<5000> > producer_ring;
  typedef boost::lockfree::spsc_queue< int,
				       boost::lockfree::capacity<5000> > consumer_ring;

  
  template<class Serializable>
  class Exchange_manager_consumer {

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
    producer_ring *produced_;
    consumer_ring *consumed_;
    double *values_;
    int previous_consumed_id_;
    bool clean_memory_;
    
  };


  #include "exchange_manager_consumer.hxx"


}




#endif // EXCHANGE_MANAGER_CONSUMER_HPP
