/**
 * @file exchange_manager_clean.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Show how to clean the exchange manager
 */
#include "shared_memory/exchange_manager_producer.hpp"
#include "shared_memory/demos/four_int_values.hpp"

#define SEGMENT_ID "exchange_demo_segment"
#define OBJECT_ID "exchange_demo_object"
#define EXCHANGE_SIZE 200

  
int main(){

  shared_memory::Exchange_manager_producer<shared_memory::Four_int_values> exchange ( SEGMENT_ID,
										      OBJECT_ID,
										      EXCHANGE_SIZE );

  exchange.clean_memory();

}
