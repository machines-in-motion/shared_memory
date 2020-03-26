
template <class Serializable, int QUEUE_SIZE>
Exchange_manager_producer<Serializable, QUEUE_SIZE>::Exchange_manager_producer(
    std::string segment_id, std::string object_id, bool leading, bool autolock)
{
    leading_ = leading;
    autolock_ = autolock;

    segment_id_ = segment_id;
    object_id_ = object_id;

    memory_.reset(new Memory(segment_id, object_id));

    if (leading)
    {
        // if leading, waiting for a consumer to change the status
        // to ready
        memory_->set_status(Memory::Status::WAITING);
    }
    else
    {
        // if not leading, informing the (leading) consumer
        // that this producer is ready
        memory_->set_status(Memory::Status::RUNNING);
    }
}

template <class Serializable, int QUEUE_SIZE>
Exchange_manager_producer<Serializable,
                          QUEUE_SIZE>::~Exchange_manager_producer()
{
    if (leading_)
    {
        memory_->clean();
    }
    else
    {
        memory_->set_status(Memory::Status::RESET);
    }
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::reset()
{
    memory_->clean();
    memory_ = NULL;
    memory_.reset(new Memory(segment_id_, object_id_));
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_producer<Serializable, QUEUE_SIZE>::ready_to_produce()
{
    typename Memory::Status status;
    memory_->get_status(status);

    if (status == Memory::Status::WAITING)
    {
        return false;
    }

    if (status == Memory::Status::RESET)
    {
        if (leading_)
        {
            // a consumer died : reseting the memory
            this->reset();
            memory_->set_status(Memory::Status::WAITING);
            return false;
        }
        else
        {
            // waiting for the leader to set back to waiting
            return false;
        }
    }

    if (status == Memory::Status::RUNNING)
    {
        return true;
    }

    return false;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::lock()
{
    memory_->lock();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::unlock()
{
    memory_->unlock();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::clear()
{
    memory_->clear();
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_producer<Serializable, QUEUE_SIZE>::set(
    const Serializable &serializable)
{
    bool everything_shared;

    if (autolock_)
    {
        this->lock();
    }

    try
    {
        everything_shared = memory_->write_serialized(serializable);
    }
    catch (const std::runtime_error &e)
    {
        if (autolock_)
        {
            this->unlock();
        }
        throw e;
    }

    if (autolock_)
    {
        this->unlock();
    }

    return everything_shared;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::get(
    std::deque<int> &get_consumed_ids)
{
    if (autolock_)
    {
        this->lock();
    }

    memory_->get_consumed_ids(get_consumed_ids);

    if (autolock_)
    {
        this->unlock();
    }

    return;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::reset_char_count()
{
    memory_->reset_char_count();
}

template <class Serializable, int QUEUE_SIZE>
int Exchange_manager_producer<Serializable, QUEUE_SIZE>::nb_char_written()
{
    return memory_->nb_char_written();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::clean_mutex(
    std::string segment_id)
{
    Exchange_manager_memory<Serializable, QUEUE_SIZE>::clean_mutex(segment_id);
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_producer<Serializable, QUEUE_SIZE>::clean_memory(
    std::string segment_id)
{
    Exchange_manager_memory<Serializable, QUEUE_SIZE>::clean_memory(segment_id);
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_producer<Serializable, QUEUE_SIZE>::producer_queue_empty()
    const
{
    return memory_->producer_queue_empty();
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_producer<Serializable, QUEUE_SIZE>::consumer_queue_empty()
    const
{
    return memory_->consumer_queue_empty();
}
