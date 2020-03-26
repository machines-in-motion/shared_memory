

namespace shared_memory
{
template <class Serializable>
Serialized_read<Serializable>::Serialized_read()
    : size_(0),
      nb_char_read_(0),
      serializable_size_(Serializer<Serializable>::serializable_size())
{
    values_ = new char[serializable_size_];
}

template <class Serializable>
Serialized_read<Serializable>::~Serialized_read()
{
    delete[] values_;
}

template <class Serializable>
void Serialized_read<Serializable>::set(char value)
{
    size_++;
    buffer_.push_back(value);
}

template <class Serializable>
bool Serialized_read<Serializable>::read(Serializable &serializable)
{
    if (size_ >= serializable_size_)
    {
        for (int i = 0; i < serializable_size_; i++)
        {
            values_[i] = buffer_.front();
            buffer_.pop_front();
            size_--;
            nb_char_read_++;
        }
        serializer_.deserialize(std::string(values_, serializable_size_),
                                serializable);
        return true;
    }
    return false;
}

template <class Serializable>
void Serialized_read<Serializable>::reset_nb_char_read()
{
    nb_char_read_ = 0;
}

template <class Serializable>
int Serialized_read<Serializable>::nb_char_read()
{
    return nb_char_read_;
}

}  // namespace shared_memory
