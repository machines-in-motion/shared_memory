#include "shared_memory/array.hpp"
#include "shared_memory/demos/item.hpp"
#include <chrono>


#define SEGMENT "benchmark_array"
#define S 100 // size of each shared_memory::Item
#define NB_ITEMS 10000 // size of array of shared_memory::Item
#define NB_ITERATIONS 250000


// what this does:
// compute the difference for writing/reading items
// to/from std::array and shared_memory::array


void run()
{
  std::array<shared_memory::Item<S>,NB_ITEMS> std_a;

  shared_memory::Item<S> item1(1);
  shared_memory::Item<S> item2;
  
  auto start = std::chrono::steady_clock::now();


  int index=6;
  
  int total_std = 0;
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      std_a[index]=item1;
      item2 = std_a[index];
      total_std += item2.get(); // making sure compiler optimization does not remove the code
    }

  auto end = std::chrono::steady_clock::now();
  long int duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  double duration_seconds = static_cast<double>(duration_us)/1e6;
  double frequency_std = static_cast<double>(NB_ITERATIONS)/duration_seconds;

  std::cout << "\n\nfrequency std: " << frequency_std << " | " << total_std << "\n\n";  

  shared_memory::array<shared_memory::Item<S>> sm_a(SEGMENT,
				     NB_ITEMS,
				     true,
				     false);


  start = std::chrono::steady_clock::now();

  int total_sm = 0;
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      sm_a.set(index,item1);
      item2 = sm_a.get(index);
      total_sm+=item2.get();
    }

  end = std::chrono::steady_clock::now();
  duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  duration_seconds = static_cast<double>(duration_us)/1e6;
  double frequency_sm = static_cast<double>(NB_ITERATIONS)/duration_seconds;

  std::cout << "\n\nfrequency sm: " << frequency_sm << " | " << total_sm << "\n\n";  

  std::cout << "\n\nratio: " << frequency_std/frequency_sm << "\n\n";
  
}

int main()
{
  run();
}
