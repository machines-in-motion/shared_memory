#include "shared_memory/shared_memory.hpp"

#define SEG "size_benchmark"

void print(const shared_memory::SegmentInfo &si)
{
  std::cout << si.get_size() << "\t" << si.get_free_memory() << "\n";
}


int main()
{

  shared_memory::clear_shared_memory(SEG);

  shared_memory::SegmentInfo si
    = shared_memory::get_segment_info(SEG);

  int previous_free = si.get_free_memory(); 

  std::cout << "initial free: " << previous_free
	    << " (" << si.get_size() << ") -> used: "
	    << si.get_size() - previous_free <<"\n";
  
  std::cout << "\nmemory used by creating new char objects\n";
      
  for(int i=0;i<3;i++)
    {
      shared_memory::set<char>(SEG,
			       std::to_string(i),
			       'a');

      si
	= shared_memory::get_segment_info(SEG);

      int free = si.get_free_memory();
      int diff = previous_free-free;
      
      std::cout << "\tmemory used: " << diff << "\n";

      previous_free = free;
      
    }

  std::cout << "\nmemory used by creating new double objects\n";

  for(int i=3;i<6;i++)
    {
      shared_memory::set<double>(SEG,
				 std::to_string(i),
				 static_cast<double>(i));

      si = shared_memory::get_segment_info(SEG);

      int free = si.get_free_memory();
      int diff = previous_free-free;
      
      std::cout << "\tmemory used: " << diff << "\n";

      previous_free = free;
      
    }

  std::cout << "\nmemory used by creating new vector<char> of ...\n";

  int index=6;
  for(int i=1;i<20;i++)
    {
      std::cout << "size:\t" << i << "\n";
      std::vector<char> v(i);
      shared_memory::set(SEG,std::to_string(index),v);
      si = shared_memory::get_segment_info(SEG);
      int free = si.get_free_memory();
      int diff = previous_free-free;
      int computed_required = 64+sizeof(char)*i;
      std::cout << "\tmemory used: " << diff
		<< " computed: " << computed_required << "\n";
      previous_free = free;
      index++;
    }

  std::cout << "\nmemory used by creating new vector<double> of ...\n";

  for(int i=1;i<20;i++)
    {
      std::cout << "size:\t" << i << "\n";
      std::vector<double> v(i);
      shared_memory::set(SEG,std::to_string(index),v);
      si = shared_memory::get_segment_info(SEG);
      int free = si.get_free_memory();
      int diff = previous_free-free;
      int computed_required = 64+sizeof(double)*i;
      std::cout << "\tmemory used: " << diff
		<< " computed: " << computed_required << "\n";
      previous_free = free;
      index++;
    }

  

  
  /*
  std::cout << "\nmemory used by creating new vector<double> of ...\n";

  for(int i=50;i<350;i+=50)
    {
      std::cout << "size:\t" << i << "\n";
      std::vector<double> v(i);
      shared_memory::set(SEG,std::to_string(index),v);
      si = shared_memory::get_segment_info(SEG);
      int free = si.get_free_memory();
      int diff = previous_free-free;
      std::cout << "\tmemory used: " << diff << "\n";
      previous_free = free;
      index++;
    }
  */
  

  std::cout << "\nfinal:\n";
  si = shared_memory::get_segment_info(SEG);
  si.print();
  
}
