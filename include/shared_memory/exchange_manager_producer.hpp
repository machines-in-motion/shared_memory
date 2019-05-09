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

  template<class Serializable>
  class Exchange_manager_producer {

  public:

    Exchange_manager_producer(std::string segment_id,
			      std::string object_id,
			      bool exception_if_not_consumed=false);

    ~Exchange_manager_producer();

    void set(const Serializable &serializable);
    bool get(std::deque<int> &get_consumed_ids);
    
  private:

    int get_last_consumed();

  private:

    std::string segment_id_;
    std::string object_id_producer_;
    std::string object_id_consumer_;
    bip::managed_shared_memory segment_;
    producer_queue *produced_;
    consumer_queue *consumed_;
    bool consumer_started_;
    double *values_;
    bool exception_if_not_consumed_;
    std::queue<int>* produced_ids_;
    
    
  };


  #include "exchange_manager_producer.hxx"


}




#endif // EXCHANGE_MANAGER_PRODUCER_HPP
