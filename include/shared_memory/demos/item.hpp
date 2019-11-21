#pragma once
#include <array>

namespace shared_memory
{

  template<int SIZE=10>
  class Item

  {
  public:

    Item()
    {
      a_.fill(0);
      v_=0;
    }
  
    Item(int value)
    {
      a_.fill(value);
      v_=value;
    }

    int get() const
    {
      return v_;
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      archive( a_ , v_ );
    }
  
    void print() const
    {
      if(SIZE>0)
	{
	  std::cout << "item: " << a_[0] << "\n";
	  return;
	}
      std::cout << "item: empty\n";
    }
  
    std::array<int,SIZE> a_;
    int v_;
  
  };

}
