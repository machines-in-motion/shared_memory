/**
 * @file non_existing_segment.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 * Gesellschaft.
 * @date 2020-10-22
 *
 * @brief checks exceptions are throwm when
 *        reading non existing segment
 */

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "shared_memory/shared_memory.hpp"

int main()
{

  std::string segment_id{"non_existing_segment_demo"};

  std::cout << "\n* clear shared memory" << std::endl;
  shared_memory::clear_shared_memory(segment_id);

  bool value;
  std::cout << "\n* get" << std::endl;
  shared_memory::get<bool>(segment_id,segment_id,value,false);

  std::cout << "\n* getting segment info" << std::endl;
  
  shared_memory::SegmentInfo si = shared_memory::get_segment_info(segment_id);
  //si.print();

  std::cout << "\n* exit" << std::endl;
  
}
