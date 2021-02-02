// Copyright (c) 2019 Max Planck Gesellschaft
// Author : Vincent Berenz

// to make sure size of segments are multiple
// of 1025
static uint get_segment_size(size_t size_array, size_t size_item)
{
    size_t total_desired_size = size_array * size_item;
    double multiplier_d = static_cast<double>(total_desired_size) / 1025.0;
    // + 1 because int rounds to lower bound
    // + 2 for memory padding
    uint multiplier = static_cast<uint>(multiplier_d) + 2;
    uint value = multiplier * 1025 * 2;
    return value;
}

template <typename T, int SIZE>
array<T, SIZE>::array(std::string segment_id,
                      std::size_t size,
                      bool clear_on_destruction,
                      bool multiprocess_safe)
    : segment_id_(segment_id),
      size_(size),
      clear_on_destruction_(clear_on_destruction),
      multiprocess_safe_(multiprocess_safe),
      mutex_(segment_id + std::string("_mutex"), clear_on_destruction)
{
    init(this->type_);
}

template <typename T, int SIZE>
array<T, SIZE>::array(const array<T, SIZE>& other)
    : segment_id_(other.segment_id_),
      size_(other.size_),
      clear_on_destruction_(other.clear_on_destruction_),
      multiprocess_safe_(other.multiprocess_safe_),
      mutex_(segment_id_ + std::string("_mutex"), clear_on_destruction_)
{
    init(this->type_);
}

template <typename T, int SIZE>
array<T, SIZE>::array(array<T, SIZE>&& other) noexcept
    : segment_id_(other.segment_id_),
      size_(other.size_),
      clear_on_destruction_(other.clear_on_destruction_),
      multiprocess_safe_(other.multiprocess_safe_),
      mutex_(segment_id_ + std::string("_mutex"), clear_on_destruction_)
{
    init(this->type_);
    other.shared_ = nullptr;
    other.clear_on_destruction_ = false;
}

template <typename T, int SIZE>
array<T, SIZE>& array<T, SIZE>::operator=(array<T, SIZE>&& other) noexcept
{
    segment_id_ = other.segment_id_;
    size_ = other.size_;
    clear_on_destruction_ = other.clear_on_destruction_;
    other.clear_on_destruction_ = false;
    multiprocess_safe_ = other.multiprocess_safe_;
    init(this->type);
    std::swap(mutex_, other.mutex_);
    other.shared_ = nullptr;
    return *this;
}

template <typename T, int SIZE>
array<T, SIZE>::~array()
{
    if (clear_on_destruction_)
    {
        clear_array(segment_id_);
    }
}

template <typename T, int SIZE>
void array<T, SIZE>::set(uint index, const T& t)
{
    set(index, t, this->type_);
}

template <typename T, int SIZE>
void array<T, SIZE>::set(uint index, const T* t)
{
    set(index, *t, this->type_);
}

template <typename T, int SIZE>
void array<T, SIZE>::get(uint index, T& t)
{
    get(index, t, this->type_);
}

template <typename T, int SIZE>
void array<T, SIZE>::get(uint index, T* t)
{
    get(index, *t, this->type_);
}

template <typename T, int SIZE>
std::string array<T, SIZE>::get_serialized(uint index)
{
    return get_serialized(index, this->type_);
}

template <typename T, int SIZE>
void array<T, SIZE>::print()
{
    SegmentInfo si(segment_manager_);
    si.print();
}

template <typename T, int SIZE>
std::size_t array<T, SIZE>::size() const
{
    return size_;
}

template <typename T, int SIZE>
void* array<T, SIZE>::get_raw()
{
    return (void*)(this->shared_);
}
