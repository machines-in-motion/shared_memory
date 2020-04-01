/**
 * @file set_data.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief shows how to serialize an instance of a class with an eigen matrix
 * as attribute
 */

#include <iostream>
#include "shared_memory/serializer.hpp"
#include "shared_memory/shared_memory.hpp"

template <int SIZE>
class Serializable
{
    typedef Eigen::Matrix<double, SIZE, 1> Vector;

public:
    Serializable() : map_(serialized_v_, SIZE, 1)
    {
    }

    void set(int index, double v)
    {
        map_[index] = v;
    }

    double get(int index)
    {
        return map_[index];
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(serialized_v_);
    }

private:
    friend shared_memory::private_serialization;

    double serialized_v_[SIZE];
    Eigen::Map<Vector> map_;
};

int main()
{
#define SIZE 100

    shared_memory::Serializer<Serializable<SIZE>> serializer;

    Serializable<SIZE> s1;
    for (int i = 0; i < SIZE; i++)
    {
        s1.set(i, static_cast<double>(i));
    }

    std::string serialized_s1 = serializer.serialize(s1);

    Serializable<SIZE> s2;
    serializer.deserialize(serialized_s1, s2);

    for (int i = 0; i < SIZE; i++)
    {
        double d = s2.get(i);
        std::cout << d << " ";
    }
    std::cout << std::endl;
}
