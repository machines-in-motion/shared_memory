

template <class Serializable, int QUEUE_SIZE>
Exchange_manager_consumer<Serializable, QUEUE_SIZE>::Exchange_manager_consumer(
    std::string segment_id, std::string object_id, bool leading, bool autolock)
{
    leading_ = leading;
    autolock_ = autolock;

    segment_id_ = segment_id;
    object_id_ = object_id;

    memory_.reset(new Memory(segment_id, object_id));

    if (leading)
    {
        // if leading, waiting for a producer to change the status
        // to ready
        memory_->set_status(Memory::Status::WAITING);
    }
    else
    {
        // if not leading, informing the (leading) producer
        // that this consumer is ready
        memory_->set_status(Memory::Status::RUNNING);
    }
}

template <class Serializable, int QUEUE_SIZE>
Exchange_manager_consumer<Serializable,
                          QUEUE_SIZE>::~Exchange_manager_consumer()
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
bool Exchange_manager_consumer<Serializable, QUEUE_SIZE>::ready_to_consume()
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
            // producer died, reseting the memory
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

    return true;
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable, QUEUE_SIZE>::lock()
{
    memory_->lock();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable, QUEUE_SIZE>::unlock()
{
    memory_->unlock();
}

template <class Serializable, int QUEUE_SIZE>
void Exchange_manager_consumer<Serializable, QUEUE_SIZE>::reset()
{
    memory_->clean();
    memory_ = NULL;
    memory_.reset(new Memory(segment_id_, object_id_));
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_consumer<Serializable, QUEUE_SIZE>::purge_feedbacks()
{
    return memory_->purge_feedbacks();
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_consumer<Serializable, QUEUE_SIZE>::consume(
    Serializable &serializable)
{
    if (autolock_)
    {
        this->lock();
    }

    bool read = false;
    try
    {
        read = memory_->read_serialized(serializable);
    }
    catch (const std::runtime_error &e)
    {
        if (autolock_)
        {
            memory_->unlock();
        }

        throw e;
    }

    if (read)
    {
        int id = serializable.get_id();
        memory_->write_serialized_id(id);
    }

    if (autolock_)
    {
        this->unlock();
    }

    return read;
}

template <class Serializable, int QUEUE_SIZE>
int Exchange_manager_consumer<Serializable, QUEUE_SIZE>::nb_char_read()
{
    return memory_->nb_char_read();
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_consumer<Serializable,
                               QUEUE_SIZE>::is_producer_queue_empty() const
{
    return memory_->producer_queue_empty();
}

template <class Serializable, int QUEUE_SIZE>
bool Exchange_manager_consumer<Serializable,
                               QUEUE_SIZE>::is_consumer_queue_empty() const
{
    return memory_->consumer_queue_empty();
}
