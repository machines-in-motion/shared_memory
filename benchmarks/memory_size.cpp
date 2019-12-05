#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

#define SEGMENT "benchmark_memory_size"


int print_segment(boost::interprocess::managed_shared_memory &segment_manager)
{
  std::cout << "size:\t" << segment_manager.get_size() << "\n"
	    << "free:\t" << segment_manager.get_free_memory() << "\n"
	    << "used:\t" << segment_manager.get_size() - segment_manager.get_free_memory() << "\n"
	    << "sanity:\t" << segment_manager.check_sanity() << "\n";

  return (segment_manager.get_size() - segment_manager.get_free_memory());
}

void test_memory_size(int total_size,
		      int obj_size)
{

  boost::interprocess::managed_shared_memory segment_manager;

  segment_manager =
	boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
						   SEGMENT,
						   total_size);

  std::cout << "\n---------------------------------------\n";
  
  std::cout << "\ntotal size: " << total_size
	    << " - nb of doubles: " << total_size/sizeof(double)
	    << "\n"
	    << "obj size: " << obj_size << "\n"
	    << "used size: " << total_size-obj_size << "\n";
  
  std::cout << "\nbefore object construction: \n";
  int used_before = print_segment(segment_manager);

  
  segment_manager.find_or_construct<char>("OBJ")[obj_size]();

  std::cout << "\nafter object construction: \n";
  int used_after = print_segment(segment_manager);
  std::cout << "\n";

  std::cout << "* usage jump: " << used_after-used_before << "\n\n";
  
  boost::interprocess::shared_memory_object::remove(SEGMENT);

}

int main()
{

  boost::interprocess::shared_memory_object::remove(SEGMENT);

  //int size = 1024*64;

  // ok -> padding of 292?
  //test_memory_size(size,size-292);
  // no ok
  //test_memory_size(size,size-291);

  int size = 1024*64*2*2*2*2*2*2*2*2*2*2*2*2*2*2;
  test_memory_size(size,size-292);

  // conclusion : memory size should be a multiple of 1024
  // with padding 292 for object size
}
