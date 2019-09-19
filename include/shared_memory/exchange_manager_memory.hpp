/**
 * @file exchange_manager_memory.hpp
 * @author Vincent Berenz (vberenz@tuebingen.mpg.de)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-06-07
 * 
 * @brief Interprocess exchange of serialized items
 */

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
#include "shared_memory/serializer.hpp"

namespace bip = boost::interprocess;

namespace shared_memory {

  template<class Serializable>
  class Serialized_read {

  public:

    Serialized_read();
    ~Serialized_read();
    void set(char value);
    bool read(Serializable &serializable);

  private:
    std::deque<char> buffer_;
    int size_;
    int serializable_size_;
    char *values_;
    Serializer<Serializable> serializer_;
    
  };



  template<class Serializable>
  class Serialized_write {

  public:

    Serialized_write();
    ~Serialized_write();
    bool empty();
    char front();
    void pop();
    bool write(const Serializable &serializable);

  private:
    std::deque<char> buffer_;
    char *values_;
    int serializable_size_;
    Serializer<Serializable> serializer_;
  };

  

  /* @brief Used internally by Exchange_manager_consumer
   * and Exchange_manager_producer. Do not use directly.
   */
  template<class Serializable, int QUEUE_SIZE>
  class Exchange_manager_memory {

    
    typedef boost::lockfree::queue<char,
				   boost::lockfree::fixed_sized<true>,
				   boost::lockfree::capacity<QUEUE_SIZE> > producer_queue;
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
    void write_serialized(const Serializable &serializable);
    void write_serialized_id(int id);
    void get_consumed_ids(std::deque<int> &get_ids);

    void set_status(Status status);
    void get_status(Status &status);
    
    void clean();

    void lock();
    void unlock();

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
    consumer_queue *consumed_;
    char *values_;
    shared_memory::Mutex locker_;
    Serialized_read<Serializable> serialized_read_;
    Serialized_write<Serializable> serialized_write_;
    int serializable_size_;
    
  };


  #include "exchange_manager_memory.hxx"

}

#endif
