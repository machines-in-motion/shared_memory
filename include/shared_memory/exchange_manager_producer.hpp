/**
 * @file exchange_manager_producer.hpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Send message in a "pipe" in the shared memory.
 */
#ifndef EXCHANGE_MANAGER_PRODUCER_HPP
#define EXCHANGE_MANAGER_PRODUCER_HPP

#include <string>
#include <deque>
#include <stdexcept>
#include <cstring>

#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializable_queue.hpp"

namespace shared_memory {

  template<class Serializable>
  class Exchange_manager_producer {

  public:

    Exchange_manager_producer(std::string segment_id,
			      std::string object_id,
			      int max_exhange_size);

    ~Exchange_manager_producer();

    bool set(const Serializable &serializable);

    bool consumer_started() const;

    void clean_memory();

    void update_memory(std::deque<int> &get_consumed_ids);
    void update_memory();
    void update_memory(std::deque<int> *get_consumed_ids);


  private:

    std::string segment_id_;
    std::string object_id_producer_;
    std::string object_id_consumer_;
    std::string object_id_reset_;
    int previous_consumer_id_;
    Serializable_queue<Serializable> items_;
    bool consumer_started_;

  };


  #include "exchange_manager_producer.hxx"


}




#endif // EXCHANGE_MANAGER_PRODUCER_HPP
