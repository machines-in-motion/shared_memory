// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz

template <typename T, int SIZE>
void array<T, SIZE>::init(FUNDAMENTAL)
{
    uint segment_size = get_segment_size(size_, sizeof(T));
    segment_manager_ = boost::interprocess::managed_shared_memory(
        boost::interprocess::open_or_create, segment_id_.c_str(), segment_size);
    this->shared_ = segment_manager_.find_or_construct<T>(
        segment_id_.c_str())[this->size_]();
}

template <typename T, int SIZE>
void array<T, SIZE>::set(uint index, const T& t, FUNDAMENTAL)
{
    if (index < 0 || index >= this->size_)
    {
        throw std::runtime_error("invalid index");
    }
    if (multiprocess_safe_)
    {
        this->mutex_.lock();
    }
    this->shared_[index] = t;
    if (multiprocess_safe_)
    {
        this->mutex_.unlock();
    }
    return;
}

template <typename T, int SIZE>
void array<T, SIZE>::get(uint index, T& t, FUNDAMENTAL)
{
    if (index < 0 || index >= this->size_)
    {
        throw std::runtime_error("invalid index");
    }
    if (multiprocess_safe_)
    {
        this->mutex_.lock();
    }
    t = this->shared_[index];
    if (multiprocess_safe_)
    {
        this->mutex_.unlock();
    }
}

template <typename T, int SIZE>
std::string array<T, SIZE>::get_serialized(uint index, FUNDAMENTAL)
{
    throw std::logic_error(
        "function not implemented for shared memory arrays encapsulating "
        "fundamental types");
}
