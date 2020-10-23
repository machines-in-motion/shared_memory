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
    std::string segment_id{"wait_for_segment_demo"};

    bool created = shared_memory::wait_for_segment(segment_id, 1);
    if (!created)
    {
        std::cout << "\nsegment was not created on time !\n"
                  << "waiting again, but not timeout"
                  << "\n";
    }
    else
    {
        std::cout << "\nsegment id " << segment_id << " exits !\n";
    }

    std::cout << "waiting  for " << segment_id << " ... \n";
    created = shared_memory::wait_for_segment(segment_id);
    std::cout << segment_id << " created !\n\n";
}
