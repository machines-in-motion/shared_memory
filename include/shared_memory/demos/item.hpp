#pragma once
#include <array>
#include <iostream>

namespace shared_memory
{
template <int SIZE = 10>
class Item

{
public:
    Item()
    {
        a_.fill(0);
        v_ = 0;
    }

    Item(int value)
    {
        a_.fill(value);
        v_ = value;
    }

    void fill(int value)
    {
        a_.fill(value);
        v_ = value;
    }

    void set(int index, int value)
    {
        a_[index] = value;
    }

    int get() const
    {
        return v_;
    }

    int get(int index) const
    {
        return a_[index];
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(a_, v_);
    }

    void compact_print() const
    {
        for (int i = 0; i < SIZE; i++)
        {
            std::cout << a_[i];
        }
        std::cout << " ";
    }

    void print() const
    {
        if (SIZE > 0)
        {
            std::cout << "item: " << a_[0] << "\n";
            return;
        }
        std::cout << "item: empty\n";
    }

    std::array<int, SIZE> a_;
    int v_;
};

}  // namespace shared_memory
