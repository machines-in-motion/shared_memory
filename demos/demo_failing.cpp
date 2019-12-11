#include "shared_memory/shared_memory.hpp"

#define TOTAL_SIZE 1140
#define SEGMENT_SIZE 12300

void set(int* shared)
{
  std::cout << "\nsetting: " << shared << "\n";
  for(int i=0;i<TOTAL_SIZE;i++)
    {
      shared[i]=1;
    }
}

void print(int *shared)
{

  std::cout << "\nprinting: " << shared << std::endl;
  
  int c=0;
  for(int i=0;i<TOTAL_SIZE;i++)
    {
      std::cout << shared[i] << " ";
      if(c==80)
	{
	  std::cout << std::endl;
	  c=0;
	}
      else
	{
	  c++;
	}
    }
  std::cout << std::endl;
}


void get_segment(int* get, bool do_set, bool do_print)
{
  boost::interprocess::managed_shared_memory segment_manager =
    boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
					       "demo_failing",
					       SEGMENT_SIZE);
  
  get = segment_manager.find_or_construct<int>("demo_failing")[TOTAL_SIZE]();


  std::cout << "\nSEGMENT: " << get << "\n";
  
  shared_memory::SegmentInfo si(segment_manager);
  std::cout << std::endl;
  std::cout << "shared: " << get << "\n";
  si.print();
  std::cout << std::endl;

  if(do_set)
    {
      set(get);
    }
  
  if(do_print)
    {
      print(get);
    }
}




int main()
{

   boost::interprocess::shared_memory_object::remove("demo_failing");

   int* s1;
   int* s2;

   // works
   get_segment(s1,true,true);
   get_segment(s2,false,true);

   // does not work
   /*
   get_segment(s1,false,false);
   set(s1);
   print(s1);
   get_segment(s2,false,false);
   set(s2);
   print(s2);
   */
}
