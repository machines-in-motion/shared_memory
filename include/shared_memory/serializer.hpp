#pragma once

// ensuring cereal is thread safe.
// https://uscilab.github.io/cereal/thread_safety.html
#define CEREAL_THREAD_SAFE 1

#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <sstream>
#include <utility>

namespace shared_memory
{
typedef cereal::access private_serialization;

template <class Serializable>
class Serializer
{
public:
    /**
     * @brief serialize an (almost) arbitrary instance to a
     * string. The method uses cereal internally and the instance
     * must implement a serialize function.
     * See for details:
     * https://uscilab.github.io/cereal/
     * Supplementary requirements:
     * - Serializable must also have a default constructor.
     * - All instances of Serializable must be of the same size.
     * (e.g. vectors must be of fixed size)
     * The generated and returned string is a private member of
     * the Serializer instance. Successive calls to serialize
     * overwrite this string.
     * @param instance to serialize to a string
     */
    const std::string& serialize(const Serializable& serializable);

    /**
     * @brief Restore the instance of serializable based on
     * the string data, which should have been generated via
     * the serialize function.
     * @param the serialized instance
     * @param instance of Serializable to be restored
     */
    void deserialize(const std::string& data, Serializable& serializable);

public:
    /**
     * Returns the serialized size (i.e. the size of the string)
     * of an instance of Serializable
     */
    static int serializable_size();

private:
    std::string data_;
};

}  // namespace shared_memory

#include "shared_memory/serializer.hxx"
