

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

template <class Serializable>
Serialized_write<Serializable>::Serialized_write()
    : nb_char_written_(0),
      serializable_size_(Serializer<Serializable>::serializable_size())
{
    values_ = new char[serializable_size_];
}

template <class Serializable>
Serialized_write<Serializable>::~Serialized_write()
{
    delete[] values_;
}

template <class Serializable>
int Serialized_write<Serializable>::nb_char_written()
{
    return nb_char_written_;
}

template <class Serializable>
void Serialized_write<Serializable>::reset_nb_char_written()
{
    nb_char_written_ = 0;
}

template <class Serializable>
bool Serialized_write<Serializable>::empty()
{
    return buffer_.empty();
}

template <class Serializable>
char Serialized_write<Serializable>::front()
{
    char value = buffer_.front();
    return value;
}

template <class Serializable>
void Serialized_write<Serializable>::pop()
{
    buffer_.pop_front();
}

template <class Serializable>
void Serialized_write<Serializable>::write(const Serializable &serializable,
                                           std::size_t expected_size)
{
    const std::string &s = serializer_.serialize(serializable);
    if (s.size() != expected_size)
    {
        throw std::runtime_error(
            "exchange_manager_memory: serialized string of unexpected size\n");
    }
    for (char c : s)
    {
        nb_char_written_++;
        buffer_.push_back(c);
    }
}

template <class Serializable, int QUEUE_SIZE>
Exchange_manager_memory<Serializable, QUEUE_SIZE>::Exchange_manager_memory(
    std::string segment_id, std::string object_id)

    : nb_char_read_(0),
      nb_char_written_(0),
      segment_(bip::open_or_create, segment_id.c_str(), 100 * 65536),
      locker_(std::string(segment_id + "_locker").c_str(), false),
      serializable_size_(Serializer<Serializable>::serializable_size())
{
    object_id_producer_ = object_id + "_producer";
    object_id_consumer_ = object_id + "_consumer";
    object_id_status_ = object_id + "_status";
    produced_ = segment_.find_or_construct<producer_queue>(
        object_id_producer_.c_str())();
    consumed_ = segment_.find_or_construct<consumer_queue>(
        object_id_consumer_.c_str())();
    segment_id_ = segment_id;
    values_ = new char[serializable_size_];

    // dev note: not sure what is happening, but the queues
    // (produced_ and consumed_) are not defined empty, despite
    // their empty() function returning true (i.e. items can
    // be poped from them). This results to various undefined behavior.
    // This function removes all these items from the queues.
    weird_purge();
}

// in constructor, remove all items from the queue, which
// can be poped from the queue despite the queue reporting
// being empty (as it should). No idea what is going on.
template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::weird_purge()
{
    lock();

    char foo;
    int foo_;

    while (produced_->empty())
    {
        bool poped = produced_->pop(foo);
        if (!poped)
        {
            break;
        }
    }

    while (consumed_->empty())
    {
        bool poped = consumed_->pop(foo_);
        if (!poped)
        {
            break;
        }
    }

    unlock();
}

template <class Serializable, int QUEUE_SIZE>
Exchange_manager_memory<Serializable, QUEUE_SIZE>::~Exchange_manager_memory()
{
    unlock();
    delete[] values_;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::set_status(
    Status status)
{
    lock();
    shared_memory::set<int>(segment_id_, object_id_status_, status);
    unlock();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::get_status(
    Status &status)
{
    int r;

    lock();
    shared_memory::get<int>(segment_id_, object_id_status_, r);
    unlock();

    status = static_cast<Status>(r);
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::clean()
{
    Exchange_manager_memory<Serializable, QUEUE_SIZE>::clean_mutex(segment_id_);
    Exchange_manager_memory<Serializable, QUEUE_SIZE>::clean_memory(
        segment_id_);
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::lock()
{
    locker_.lock();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::unlock()
{
    locker_.unlock();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::clean_mutex(
    std::string segment_id)
{
    // mutex destruction called in destructor
    shared_memory::Mutex m(segment_id + "_locker", true);
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::clean_memory(
    std::string segment_id)
{
    shared_memory::clear_shared_memory(segment_id);
    shared_memory::delete_segment(segment_id);
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_memory<Serializable, QUEUE_SIZE>::read_serialized(
    Serializable &serializable)
{
    while (!produced_->empty())
    {
        char value;
        bool poped = produced_->pop(value);
        if (poped)
        {
            nb_char_read_++;
            serialized_read_.set(value);
        }
        else
        {
            break;
        }
    }

    bool read = serialized_read_.read(serializable);

    return read;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::clear()
{
    char foo;
    while (!produced_->empty())
    {
        produced_->pop(foo);
    }
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_memory<Serializable, QUEUE_SIZE>::write_serialized(
    const Serializable &serializable)
{
    bool everything_shared = true;

    serialized_write_.write(serializable, serializable_size_);

    while (!serialized_write_.empty())
    {
        char value = serialized_write_.front();
        bool pushed = produced_->bounded_push(value);

        if (pushed)
        {
            nb_char_written_++;
            serialized_write_.pop();
        }
        else
        {
            everything_shared = false;
            break;
        }
    }

    return everything_shared;
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_memory<Serializable, QUEUE_SIZE>::purge_feedbacks()
{
    while (!consumed_buffer_.empty())
    {
        int id = consumed_buffer_.front();
        bool pushed = consumed_->push(id);
        if (!pushed)
        {
            return false;
        }
        else
        {
            consumed_buffer_.pop_front();
        }
    }

    return true;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::write_serialized_id(
    int id)
{
    // if some consumed id could previously not be set in the shared memory
    // (because this latest was full), they have been buffered
    // in consumed_buffer_. Trying to purge it.
    purge_feedbacks();

    // trying to push id that was read into consumed_,
    // to inform the producer that this item has been consumed
    bool pushed = consumed_->push(id);

    // if consumed_ is full, we buffer
    if (!pushed)
    {
        consumed_buffer_.push_back(id);
    }
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::get_consumed_ids(
    std::deque<int> &get_consumed_ids)
{
    int id;
    bool has_consumed;

    while (has_consumed)
    {
        has_consumed = consumed_->pop(id);

        if (has_consumed)
        {
            get_consumed_ids.push_back(id);
        }
    }
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_memory<Serializable, QUEUE_SIZE>::reset_char_count()
{
    nb_char_read_ = 0;
    nb_char_written_ = 0;
}

template <class Serializable, int QUEUE_SIZE>
int Exchange_manager_memory<Serializable, QUEUE_SIZE>::nb_char_written()
{
    return nb_char_written_;
}

template <class Serializable, int QUEUE_SIZE>
int Exchange_manager_memory<Serializable, QUEUE_SIZE>::nb_char_read()
{
    return nb_char_read_;
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_memory<Serializable, QUEUE_SIZE>::consumer_queue_empty()
    const
{
    return consumed_->empty();
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_memory<Serializable, QUEUE_SIZE>::producer_queue_empty()
    const
{
    return produced_->empty();
}
