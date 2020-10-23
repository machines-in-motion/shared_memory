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
    shared_memory::clear_shared_memory(segment_id);

    std::cout << "\nsegment created !\n";
    shared_memory::set<double>(segment_id, segment_id, 1.0);

    usleep(1000000);
    std::cout << "\ndestroying segment\n\n";
    shared_memory::clear_shared_memory(segment_id);
}
