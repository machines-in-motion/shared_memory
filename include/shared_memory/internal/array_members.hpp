// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz

#pragma once

// This is non public support code for shared_memory::array

namespace shared_memory
{
typedef std::integral_constant<int, 0> SERIALIZABLE;
typedef std::integral_constant<int, 1> FUNDAMENTAL;
typedef std::integral_constant<int, 2> FUNDAMENTAL_ARRAY;

namespace internal
{
// defining members for serializable instances,
// which will be stored as char arrays
template <typename T, int SIZE = 0, typename Enable = void>
class array_members
{
protected:
    Serializer<T> serializer_;
    char* shared_;
    std::size_t item_size_;
    std::size_t total_size_;
    SERIALIZABLE type_;
};

// defining members for fundamental types
// which will directly be stored
template <typename T>
class array_members<
    T,
    0,
    typename std::enable_if<std::is_fundamental<T>::value>::type>
{
protected:
    T* shared_;
    FUNDAMENTAL type_;
};

// defining members for array of fundamental types,
// which will be directly stored
template <typename T, int SIZE>
class array_members<
    T,
    SIZE,
    typename std::enable_if<std::is_fundamental<T>::value && SIZE != 0>::type>
{
protected:
    T* shared_;
    std::size_t total_size_;
    FUNDAMENTAL_ARRAY type_;
};

}  // namespace internal

}  // namespace shared_memory
