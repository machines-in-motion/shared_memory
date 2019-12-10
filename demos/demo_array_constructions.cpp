
#include "shared_memory/array.hpp"

#define PER_ARRAY 76


void print(shared_memory::array<int,PER_ARRAY> &v, int size)
{
  int v_[PER_ARRAY];
  std::cout << "\nArray -->\n";
  for(int i=0;i<size;i++)
    {
      v.get(i,*v_);
      std::cout << "\tarray:";
      for(int j=0;j<PER_ARRAY;j++)
	{
	  std::cout << v_[j] << " ";
	}
      std::cout << "\n";
    }
  std::cout << "<-- Array\n";
}

int main()
{

  int size=15;

  /*
  shared_memory::clear_array("test_array");
  shared_memory::array<int> a("test_array",size,true,true);
  shared_memory::array<int> b(a);*/
  
  shared_memory::clear_array("test_array");
  shared_memory::array<int,PER_ARRAY> c("test_array",size,true,true);

  int values[PER_ARRAY];
  for(int i=0;i<size;i++)
    {
      for(uint j=0;j<PER_ARRAY;j++)
	{
	  values[j]=(i);
	}
      c.set(i,*values);
    }

  print(c,size);

  std::cout << "\n\n---------------\n\n";
  
  shared_memory::array<int,PER_ARRAY> d(c);
  print(d,size);

  std::cout << "\nexit\n";
  
  
}
