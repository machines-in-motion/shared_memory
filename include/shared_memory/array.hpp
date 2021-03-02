// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz

#pragma once

#include "shared_memory/mutex.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/shared_memory.hpp"

// This header
// - defines types SERIALIZABLE, FUNDAMENTAL and FUNDAMENTAL_ARRAY
// - defines super class array_member, which declares the private
// members specific for each implementation (serializable,
// fundamental and fundamental array)
#include "shared_memory/internal/array_members.hpp"

#include <cstring>
#include <stdexcept>

namespace shared_memory
{
/**
 * wipe the shared memory segment created by an instance
 * of shared_memory::array, including mutexes, if any.
 * If there are no memory segment of this id, there will be
 * no effect. If shared_memory::array instances are pointing to the
 * wiped out segment, their get and set functions may hang
 * indefinitely.
 */
void clear_array(std::string segment_id);

/**
 * Implement a shared array stored on a shared memory
 * segment. Items hosted by the array may be of (1) fundamental
 * type (e.g. int, double, char), (2) array of fundamental type
 * (e.g. int[10]); or (3) instances of a class implementing a
 * serializable function (see shared_memory::serializer).
 */
template <typename T, int SIZE = 0>
class array : public internal::array_members<T, SIZE>
{
private:
    // ------------------------------------
    // implementation for fundamental types
    // ------------------------------------

    void init(FUNDAMENTAL);
    void set(uint index, const T& t, FUNDAMENTAL);
    void get(uint index, T& t, FUNDAMENTAL);
    std::string get_serialized(uint index,
                               FUNDAMENTAL);  // throws std::logic_error

    // ----------------------------------------------
    // implementation for arrays of fundamental types
    // ----------------------------------------------

    void init(FUNDAMENTAL_ARRAY);
    void set(uint index, const T& t, FUNDAMENTAL_ARRAY);
    void get(uint index, T& t, FUNDAMENTAL_ARRAY);
    std::string get_serialized(uint index,
                               FUNDAMENTAL_ARRAY);  // throws std::logic_error

    // -----------------------------------------
    // implementation for serializable instances
    // -----------------------------------------

    void init(SERIALIZABLE);
    void set(uint index, const T& t, SERIALIZABLE);
    void get(uint index, T& t, SERIALIZABLE);
    std::string get_serialized(uint index, SERIALIZABLE);

public:
    /**
     * @param segment_id should be the same for
     * all array pointing to the same shared memory segment
     * @param size : number of elements to be stored by the array
     * @param clear_on_destruction: if true, the shared memory segment
     * will be wiped on destruction of the array. Note that any other array
     * pointing to this segment may hang indefinitely as a result. If
     * no arrays pointing to the shared memory segment delete the segment,
     * then users are expected to call shared_memory::clear_array. Failing
     * to do so may result in new array pointing to a new memory segment
     * of the same id to hang indefinitely at construction.
     * @param multiprocess_safe if false, it is strongly adviced to
     * protect accesses via a shared_memory::Mutex
     */
    array(std::string segment_id,
          std::size_t size,
          bool clear_on_destruction = true,
          bool multiprocess_safe = true);

    /**
     * wipe the related shared memory segment
     * if clear_on_destruction is true (true by default)
     */
    ~array();

    /**
     * this array and other array will point to the
     * same memory segment, and will have same values for
     * clear_on_destruction and multiprocess_safe
     */
    array(const array<T, SIZE>& other);

    /**
     * This array will point to the share memory segment
     * pointed at by other; and will have same value for
     * multprocess_safe and clear_on_destruction.
     * Warning: even if other.clear_on_destruction is
     * true, the segment memory will not be wiped on the destruction
     * of other. The duty of deleting the shared memory is passed
     * to the new instance, so to speak
     */
    array(array<T, SIZE>&& other) noexcept;

    /**
     * This array will point to the share memory segment
     * pointed at by other; and will have same value for
     * multprocess_safe and clear_on_destruction.
     * Warning: even if other.clear_on_destruction is
     * true, the segment memory will not be wiped on the destruction
     * of other. The duty of deleting the shared memory is passed
     * to the new instance, so to speak
     */
    array<T, SIZE>& operator=(array<T, SIZE>&& other) noexcept;

    /**
     * @brief set element t at index
     */
    void set(uint index, const T& t);

    /**
     * @brief set element t at index
     */
    void set(uint index, const T* t);

    /**
     * @brief read element at index into t
     */
    void get(uint index, T& t);

    /**
     * @brief read element at index into t
     */
    void get(uint index, T* t);

    /**
     * @brief max number of elements in the array
     */
    std::size_t size() const;

    /**
     * @brief return the serialized string representation of
     * the element if T is a serializable class. Throws a logic error otherwise.
     */
    std::string get_serialized(uint index);

    /**
     * @brief print in terminal info about array's
     * memory usage
     */
    void print();

    // for debug
    void* get_raw();

private:
    boost::interprocess::managed_shared_memory segment_manager_;
    std::string segment_id_;
    std::size_t size_;           // number of elements in array
    bool clear_on_destruction_;  // memory segment will be clear at destruction
                                 // if true
    bool multiprocess_safe_;     // protects all operation with an interprocess
                                 // mutex if true
    shared_memory::Mutex mutex_;
};

// code common for all implementations
#include "array.hxx"

// implementation for array of fundamental types
// (e.g. array of int or of double)
#include "array_fundamental.hxx"

// implementation for array of array of fundamental
// types
// (e.g. array of array of int, possibly useful to
// store images)
#include "array_fundamental_array.hxx"

// implementation for arbitrary class implementing
// the serialize function (see shared_memory/serializer.hpp)
#include "array_serializable.hxx"

}  // namespace shared_memory
