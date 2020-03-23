/**
 * @file four_int_values.hpp
o * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Defines a messages to be sent throw the shared memory
 */

#pragma once

#include <iostream>
#include "shared_memory/serializer.hpp"

namespace shared_memory
{
/**
 * Example of an instance that can be serialized.
 * Notice:
 * There is a default constructor
 * There is a serialize function
 * It is friend to private_serialization
 */
class Four_int_values
{
public:
    Four_int_values();
    Four_int_values(int a, int b, int c, int d);
    int get_id() const;
    void set_id(int id);

public:
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(id_, values_);
    }

public:
    // stored values are all equals, in same order
    bool equal(const Four_int_values &other) const;

    // equal, + same id
    bool same(const Four_int_values &other) const;

public:
    void print() const;

private:
    static int next_id();

private:
    friend private_serialization;

    std::vector<int> values_;
    int id_;
};

}  // namespace shared_memory
