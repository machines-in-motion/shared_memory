/**
 * @file exceptions.h
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Defines debugging exceptions for this package.
 */
#ifndef SHARED_MEMORY_EXCEPTION_HPP
#define SHARED_MEMORY_EXCEPTION_HPP

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace shared_memory
{
class Allocation_exception : public std::exception
{
public:
    Allocation_exception(const std::string &segment_id,
                         const std::string &object_id);
    ~Allocation_exception() throw();
    const char *what() const throw();

private:
    std::string error_message_;
};
class Non_existing_segment_exception : public std::exception
{
public:
    Non_existing_segment_exception(const std::string &segment_id);
    ~Non_existing_segment_exception() throw();
    const char *what() const throw();

private:
    std::string error_message_;
};
class Memory_overflow_exception : public std::exception
{
public:
    Memory_overflow_exception(const std::string error_message);
    ~Memory_overflow_exception() throw();
    const char *what() const throw();

private:
    std::string error_message_;
};

class Unexpected_size_exception : public std::exception
{
public:
    Unexpected_size_exception(const std::string &segment_id,
                              const std::string &object_id,
                              int expected_size,
                              int size_given);
    ~Unexpected_size_exception() throw();
    const char *what() const throw();

private:
    std::string error_message_;
};

class Not_consumed_exception : public std::exception
{
public:
    Not_consumed_exception(int missed_id);
    ~Not_consumed_exception() throw();
    const char *what() const throw();

private:
    std::string error_message_;
};

template <typename Key>
class Unexpected_map_key : public std::exception
{
public:
    Unexpected_map_key(const std::string &segment_id,
                       const std::string &object_id,
                       Key &expected_key)
    {
        std::ostringstream s;
        s << "shared_memory : none existing key in map when setting/getting "
          << segment_id << " (" << object_id << "): "
          << "expected key: " << expected_key;
        this->error_message_ = s.str();
    }

    ~Unexpected_map_key() throw()
    {
    }

    const char *what() const throw()
    {
        return this->error_message_.c_str();
    }

private:
    std::string error_message_;
};
}  // namespace shared_memory

#endif  // SHARED_MEMORY_EXCEPTION_HPP
