/**
 * @file exceptions.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Defines some exception specific to this API
 */
#include "shared_memory/exceptions.h"

namespace shared_memory
{
Allocation_exception::Allocation_exception(const std::string &segment_id,
                                           const std::string &object_id)
{
    std::ostringstream s;
    s << "shared_memory : ran out of memory when allocating " << segment_id
      << " (" << object_id << ")";
    this->error_message_ = s.str();
}

Allocation_exception::~Allocation_exception() throw()
{
}

const char *Allocation_exception::what() const throw()
{
    return this->error_message_.c_str();
}

Memory_overflow_exception::Memory_overflow_exception(
    const std::string error_message)
{
    this->error_message_ = error_message;
}

Memory_overflow_exception::~Memory_overflow_exception() throw()
{
}

const char *Memory_overflow_exception::what() const throw()
{
    return this->error_message_.c_str();
}

Not_consumed_exception::Not_consumed_exception(int missed_id)
{
    std::ostringstream s;
    s << "shared memory exchange manager : "
      << "at least one item was produced but not consumed: " << missed_id
      << "\n";
    this->error_message_ = s.str();
}

Not_consumed_exception::~Not_consumed_exception() throw()
{
}

const char *Not_consumed_exception::what() const throw()
{
    return this->error_message_.c_str();
}

Unexpected_size_exception::Unexpected_size_exception(
    const std::string &segment_id,
    const std::string &object_id,
    int expected_size,
    int size_given)
{
    std::ostringstream s;
    s << "shared_memory : size error when setting/getting " << segment_id
      << " (" << object_id << "): expected size: " << expected_size
      << " provided size: " << size_given;
    this->error_message_ = s.str();
}

Unexpected_size_exception::~Unexpected_size_exception() throw()
{
}

const char *Unexpected_size_exception::what() const throw()
{
    return this->error_message_.c_str();
}
}  // namespace shared_memory
