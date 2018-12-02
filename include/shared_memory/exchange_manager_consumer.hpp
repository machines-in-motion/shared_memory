#ifndef EXCHANGE_MANAGER_CONSUMER_HPP
#define EXCHANGE_MANAGER_CONSUMER_HPP

#include <string>
#include "shared_memory/shared_memory.hpp"
#include "shared_memory/serializable_stack.hpp"

namespace shared_memory {


  template<class Serializable>
  class Exchange_manager_consumer {

  public:

    Exchange_manager_consumer(std::string segment_id,
			      std::string object_id,
			      int max_exhange_size);

    
    ~Exchange_manager_consumer();

    void clean_memory();
    
    void update_memory(bool verbose=false);
    
    bool empty();

    bool ready_to_consume();
    
    void consume(Serializable &serializable);


  private:
    void reset_if_producer_stopped();
      
  private:

    std::string segment_id_;
    std::string object_id_producer_;
    std::string object_id_consumer_;
    std::string object_id_reset_;
    Serializable_stack_reader<Serializable> items_;
    int consumer_id_;
    int previous_producer_id_;
    int nb_elements_;
    int nb_consumed_;
    bool ready_to_consume_;
    
  };

  
  #include "exchange_manager_consumer.hxx"
  

}




#endif // EXCHANGE_MANAGER_CONSUMER_HPP
