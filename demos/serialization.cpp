#include "shared_memory/serializer.hpp"
#include "shared_memory/shared_memory.hpp"

class SerializableExample
{
public:
    SerializableExample()
    {
    }

    SerializableExample(int i1, int i2, double d1)
    {
        i1_ = i1;
        i2_ = i2;
        d1_ = d1;
    }

    void add(int value)
    {
        v_.push_back(value);
    }

    // This method lets cereal know which data members to serialize
    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(i1_, i2_, d1_, v_);
    }

    void print()
    {
        std::cout << i1_ << " " << i2_ << " " << d1_ << "\n";
        for (size_t i = 0; i < v_.size(); i++)
        {
            std::cout << v_[i] << " ";
        }
        std::cout << "\n";
    }

private:
    // this is required for the serialization
    // to access the private members
    friend shared_memory::private_serialization;

    int i1_, i2_;
    double d1_;
    std::vector<int> v_;
};

int main()
{
    shared_memory::clear_shared_memory("s1");

    SerializableExample in(1, 1, 4);
    in.add(5);
    in.add(6);
    in.print();
    shared_memory::serialize("s1", "o1", in);

    SerializableExample out;
    shared_memory::deserialize("s1", "o1", out);
    out.print();
}
