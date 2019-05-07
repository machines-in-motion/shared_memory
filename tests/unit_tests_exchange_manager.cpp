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


class Exchange_manager_tests : public ::testing::Test {

protected:

  void SetUp() {
    clear_memory();
    shared_memory::get_segment_mutex(SEGMENT_ID).unlock();
  }

  void TearDown() {
    clear_memory();
  }
  
};





