/**
 * @file serializable_exchange.hpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Interface to a serializable object
 */
#ifndef SERIALIZABLE_EXCHANGE_HPP
#define SERIALIZABLE_EXCHANGE_HPP

#include <cstring>
#include <deque>
#include <stdexcept>
#include <string>

#include "shared_memory/serializable.hpp"
#include "shared_memory/shared_memory.hpp"

namespace shared_memory
{
template <class Serializable>
class Serializable_exchange
{
public:
    Serializable_exchange(std::string segment_id, std::string object_id);

    ~Serializable_exchange();

    void set(const Serializable &serializable);
    void read(Serializable &serializable);

private:
    std::string segment_id_;
    std::string object_id_;
    double *data_;
};

#include "serializable_exchange.hxx"

}  // namespace shared_memory

#endif
