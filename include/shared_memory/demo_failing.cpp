#include "shared_memory/shared_memory.hpp"

#define TOTAL_SIZE 1140
#define SEGMENT_SIZE 12300

int * get_segment()
{
  boost::interprocess::managed_shared_memory segment_manager =
    boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
					       "demo_failing",
					       SEGMENT_SIZE);
  
  return segment_manager.find_or_construct<int>("demo-failing")[TOTAL_SIZE]();
}


void set(int* shared)
{
  for(int i=0;i<TOTAL_SIZE;i++)
    {
      shared[i]=1;
    }
}

void print(int *shared)
{
  int c=0;
  for(int i=0;i<TOTAL_SIZE;i++)
    {
      std::cout << shared[i] << " ";
      if(c==20)
	{
	  std::cout << std::endl;
	}
      c++;
    }
}


int main()
{
  
  boost::interprocess::shared_memory_object::remove("demo_failing");
  
  int* s1 = get_segment();
  set(s1);
  print(s1);
  int* s2 = get_segment();
  print(s2);
}
