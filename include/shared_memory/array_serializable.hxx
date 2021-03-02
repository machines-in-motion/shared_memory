// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz

template <typename T, int SIZE>
void array<T, SIZE>::init(SERIALIZABLE)
{
    this->item_size_ = Serializer<T>::serializable_size();
    this->total_size_ = this->item_size_ * this->size_;
    uint segment_size = get_segment_size(this->total_size_, sizeof(char));
    segment_manager_ = boost::interprocess::managed_shared_memory(
        boost::interprocess::open_or_create, segment_id_.c_str(), segment_size);
    this->shared_ = segment_manager_.find_or_construct<char>(
        segment_id_.c_str())[this->total_size_]();
}

template <typename T, int SIZE>
void array<T, SIZE>::set(uint index, const T& t, SERIALIZABLE)
{
    uint abs_index = index * this->item_size_;
    if (abs_index < 0 || index >= this->total_size_)
    {
        throw std::runtime_error("invalid index");
    }
    if (multiprocess_safe_)
    {
        mutex_.lock();
    }
    const std::string& serialized = this->serializer_.serialize(t);
    for (uint index = 0; index < this->item_size_; index++)
    {
        this->shared_[abs_index + index] = serialized[index];
    }
    if (multiprocess_safe_)
    {
        mutex_.unlock();
    }
}

template <typename T, int SIZE>
void array<T, SIZE>::get(uint index, T& t, SERIALIZABLE)
{
    uint abs_index = index * this->item_size_;
    if (abs_index < 0 || abs_index >= this->total_size_)
    {
        throw std::runtime_error("invalid index");
    }
    if (multiprocess_safe_)
    {
        mutex_.lock();
    }
    this->serializer_.deserialize(
        std::string(&this->shared_[abs_index], this->item_size_), t);
    if (multiprocess_safe_)
    {
        mutex_.unlock();
    }
}

template <typename T, int SIZE>
std::string array<T, SIZE>::get_serialized(uint index, SERIALIZABLE)
{
    uint abs_index = index * this->item_size_;
    if (abs_index < 0 || abs_index >= this->total_size_)
    {
        throw std::runtime_error("invalid index");
    }
    if (multiprocess_safe_)
    {
        mutex_.lock();
    }
    std::string r(&this->shared_[abs_index], this->item_size_);
    if (multiprocess_safe_)
    {
        mutex_.unlock();
    }
    return r;
}
