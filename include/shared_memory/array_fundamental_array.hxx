// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz

template <typename T, int SIZE>
void array<T, SIZE>::init(FUNDAMENTAL_ARRAY)
{
    this->total_size_ = size_ * SIZE;

    uint segment_size = get_segment_size(this->total_size_, sizeof(T));

    segment_manager_ = boost::interprocess::managed_shared_memory(
        boost::interprocess::open_or_create, segment_id_.c_str(), segment_size);
    this->shared_ = segment_manager_.find_or_construct<T>(
        segment_id_.c_str())[this->total_size_]();
}

template <typename T, int SIZE>
void array<T, SIZE>::set(uint index, const T& t, FUNDAMENTAL_ARRAY)
{
    if (index >= this->size_)
    {
        throw std::runtime_error("invalid index");
    }
    if (multiprocess_safe_)
    {
        mutex_.lock();
    }
    uint abs_index = index * SIZE;
    uint c = 0;
    for (uint i = abs_index; i < abs_index + SIZE; i++)
    {
        this->shared_[i] = (&t)[c++];
    }
    if (multiprocess_safe_)
    {
        mutex_.unlock();
    }
}

template <typename T, int SIZE>
void array<T, SIZE>::get(uint index, T& t, FUNDAMENTAL_ARRAY)
{
    if (index > size_)
    {
        throw std::runtime_error("invalid index");
    }
    if (multiprocess_safe_)
    {
        mutex_.lock();
    }
    uint abs_index = index * SIZE;
    uint c = 0;
    for (uint i = abs_index; i < abs_index + SIZE; i++)
    {
        (&t)[c++] = this->shared_[i];
    }
    if (multiprocess_safe_)
    {
        mutex_.unlock();
    }
}

template <typename T, int SIZE>
std::string array<T, SIZE>::get_serialized(uint index, FUNDAMENTAL_ARRAY)
{
    throw std::logic_error(
        "function not implemented for shared memory arrays encapsulating array "
        "of fundamental types");
}
