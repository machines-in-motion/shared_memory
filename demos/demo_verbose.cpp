/**
 * @file set_data.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief shows how to turn of console prints
 */


#include "shared_memory/shared_memory.hpp"


int main()
{

  shared_memory::clear_shared_memory("verbose_demo");
  
  shared_memory::set("verbose_demo","verbose_demo1",1);

  shared_memory::set_verbose(false);

  shared_memory::set("verbose_demo","verbose_demo2",2);

}
