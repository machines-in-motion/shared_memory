#include "shared_memory/exchange_manager_producer.hpp"
#include "shared_memory/exchange_manager_consumer.hpp"
#include "shared_memory/demos/four_int_values.hpp"
#include "shared_memory/tests/tests.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <memory>

#define SEGMENT_ID "exchange_manager_ut_segment"
#define OBJECT_ID "exchange_manager_ut_object"
#define BUFFER_SIZE 100

using namespace shared_memory;


static inline void clear_memory(){
  shared_memory::clear_shared_memory(SEGMENT_ID);
}


Exchange_manager_producer<Four_int_values> *producer;
Exchange_manager_consumer<Four_int_values> *consumer;


class Exchange_manager_tests : public ::testing::Test {

protected:

  void SetUp() {
    clear_memory();
    shared_memory::get_segment_mutex(SEGMENT_ID).unlock();
    producer = new Exchange_manager_producer<Four_int_values>(SEGMENT_ID,
							      OBJECT_ID,
							      BUFFER_SIZE);
    consumer = new Exchange_manager_consumer<Four_int_values>(SEGMENT_ID,
							      OBJECT_ID,
							      BUFFER_SIZE);
  }

  void TearDown() {
    delete producer;
    delete consumer;
    clear_memory();
  }
  
};


TEST_F(Exchange_manager_tests,write_and_read){
  std::deque<int> consumed;
  Four_int_values p1(1,2,3,4);
  Four_int_values p2(5,6,7,8);
  producer->set(p1);
  producer->set(p2);
  producer->update_memory(consumed);
  ASSERT_EQ(consumed.size(),0);
  consumer->update_memory();
  Four_int_values c1;
  Four_int_values c2;
  consumer->consume(c1);
  consumer->consume(c2);
  consumer->update_memory();
  ASSERT_EQ(true,p1.same(c1));
  ASSERT_EQ(true,p2.same(c2));
  producer->update_memory(consumed);
  ASSERT_EQ(consumed.size(),2);
  ASSERT_EQ(consumed[0],p1.get_id());
  ASSERT_EQ(consumed[1],p2.get_id());
}



