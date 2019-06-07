#ifndef EXCHANGE_MANAGER_MEMORY_HPP
#define EXCHANGE_MANAGER_MEMORY_HPP

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


  /* @brief Used internally by Exchange_manager_consumer
   * and Exchange_manager_producer. Do not use directly.
   */
  template<class Serializable, int QUEUE_SIZE>
  class Exchange_manager_memory {

    
    typedef boost::lockfree::queue<double,
				   boost::lockfree::fixed_sized<true>,
				   boost::lockfree::capacity<Serializable::serialization_size*QUEUE_SIZE> > producer_queue;
    typedef boost::lockfree::queue<int,
				   boost::lockfree::fixed_sized<true>,
				   boost::lockfree::capacity<QUEUE_SIZE> > consumer_queue;


  public:

    enum Status {RESET,WAITING,RUNNING};
    
  public:


    Exchange_manager_memory(std::string segment_id,
			    std::string object_id);
    ~Exchange_manager_memory();

    bool read_serialized(Serializable &serializable);
    bool write_serialized(const Serializable &serializable);
    void write_serialized_id(int id);
    void get_consumed_ids(std::deque<int> &get_ids);

    void set_status(Status status);
    void get_status(Status &status);
    
    void clean();

    void lock();
    void unlock();

  private:

    void set_consumed_memory();
    
  public:

    static void clean_mutex( std::string segment_id );
    static void clean_memory( std::string segment_id );

    
  public:

    std::string segment_id_;
    std::string object_id_producer_;
    std::string object_id_consumer_;
    std::string object_id_status_;
    bip::managed_shared_memory segment_;
    producer_queue *produced_;
    std::deque<int> consumed_buffer_;
    std::deque<int> consumed_memory_;
    consumer_queue *consumed_;
    double *values_;
    shared_memory::Mutex locker_;
    int producer_size_;

  };


  #include "exchange_manager_memory.hxx"

}

#endif
