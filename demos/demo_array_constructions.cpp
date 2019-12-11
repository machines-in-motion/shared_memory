
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

  shared_memory::clear_array("test_array");
  shared_memory::array<int,PER_ARRAY> c("test_array",size,true,true);

  std::cout << "\nmemory segment after construction:\n";
  c.print();
  std::cout << "\n";

  std::cout << "\nvalues before attribution:\n";
  print(c,size);
  std::cout << "\n";
  
  int values[PER_ARRAY];
  for(int i=0;i<size;i++)
    {
      for(uint j=0;j<PER_ARRAY;j++)
	{
	  values[j]=1;
	}
      c.set(i,*values);
    }

  std::cout << "\nvalues after attribution:\n";
  print(c,size);
  std::cout << "\n";
  
  //shared_memory::array<int,PER_ARRAY> d(c);
  shared_memory::array<int,PER_ARRAY> d("test_array",size,true,true);
  
  std::cout << "\ncopied values:\n";
  print(d,size);
  std::cout << "\n";

  std::cout << "\nvalues after copy:\n";
  print(c,size);
  std::cout << "\n";

  std::cout << "\nmemory segment:\n";
  d.print();
  c.print();
  
  std::cout << "\nexit\n";
  
  
}
