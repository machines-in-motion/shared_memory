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


class SM
{
  
public:

  SM(bool do_set, bool do_print)
  {
    get_segment(do_set,do_print);
  }

  void sm_set()
  {
    set(shared_);
  }

  void sm_print()
  {
    print(shared_);
  }

  bool same(SM &other)
  {
    return shared_ == other.shared_;
  }
  
private:
  
  void get_segment(bool do_set, bool do_print)
  {
    segment_manager_ =
      boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
						 "demo_failing",
						 SEGMENT_SIZE);
  
    shared_ = segment_manager_.find_or_construct<int>("demo_failing")[TOTAL_SIZE]();


    std::cout << "\nSEGMENT: " << shared_ << "\n";
  
    shared_memory::SegmentInfo si(segment_manager_);
    std::cout << std::endl;
    std::cout << "shared: " << shared_ << "\n";
    si.print();
    std::cout << std::endl;

    if(do_set)
      {
	set(shared_);
      }
  
    if(do_print)
      {
	print(shared_);
      }
  }

  
  boost::interprocess::managed_shared_memory segment_manager_;
  int * shared_;
  
};



int main()
{

   boost::interprocess::shared_memory_object::remove("demo_failing");

   // I think failing (not same pointer address)
   //SM sm1(true,true);
   //SM sm2(false,true);

   // working ?
   SM sm1(false,false);
   sm1.sm_set();
   sm1.sm_print();
   SM sm2(false,false);
   sm2.sm_print();

   std::cout << "\nsame ? " << sm1.same(sm2) << "\n\n";

   
   
}
