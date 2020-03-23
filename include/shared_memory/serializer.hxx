
namespace shared_memory
{
template <class Serializable>
const std::string& Serializer<Serializable>::serialize(
    const Serializable& serializable)
{
    std::stringstream ss;
    cereal::BinaryOutputArchive boa(ss);
    boa(serializable);
    data_ = ss.str();
    return data_;
}

template <class Serializable>
void Serializer<Serializable>::deserialize(const std::string& data,
                                           Serializable& serializable)
{
    std::stringstream ss(data);
    cereal::BinaryInputArchive bia(ss);
    bia(serializable);
}

template <class Serializable>
int Serializer<Serializable>::serializable_size()
{
    Serializable serializable;
    Serializer<Serializable> serializer;
    const std::string& data = serializer.serialize(serializable);
    return data.size();
}

}  // namespace shared_memory
