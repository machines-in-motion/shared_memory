#include "shared_memory/array.hpp"
#include "shared_memory/demos/item.hpp"
#include <chrono>


#define SEGMENT "benchmark_array"
#define NB_ITERATIONS 250000


class FrequencyMeasure
{
public:
  static void start()
  {
    start_= std::chrono::steady_clock::now();
    tick_ = 0;
  }
  static void tick()
  {
    tick_++;
  }
  double end()
  {
    auto end = std::chrono::steady_clock::now();
    long int duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end-start_).count();
    double duration_seconds = static_cast<double>(duration_us)/1e6;
    double frequency = static_cast<double>(NB_ITERATIONS)/duration_seconds;
    return frequency;
  }
protected:
  static std::chrono::time_point<std::chrono::steady_clock> start_;
  static long int tick_;
};


void report(double f_std, int c_std,
	    double f_sm, int c_sm)
{
  std::cout << "std: " << f_std << " (" << c_std << ")"
	    << "\t|\t"
	    << "sm: " << f_sm << " (" << c_sm << ") "
	    << "\t|\t"
	    << f_std / f_sm << "\n";
}

// what this does:
// compute the difference for writing/reading items
// to/from std::array and shared_memory::array

  

template<int NB_ITEMS>
void run()
{

  shared_memory::array<int>::clear(SEGMENT);
  
  
  // testing array of ints
  // ---------------------
  
  // std
  
  std::array<int,NB_ITEMS> std_ints;
  FrequencyMeasure::start();
  int index=0;
  int c_std=0;
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      std_ints[index]=iteration;
      int value = std_ints[index];
      c_std+=value; // making sure compiler optimization does not remove the code
      index++;
      if(index>=NB_ITEMS)
	index=0;
    }
  double f_std = FrequencyMeasure::end();

  // shared_memory
  
  shared_memory::array<int> sm_ints(SEGMENT,NB_ITEMS);
  FrequencyMeasure::start();
  index=0;
  int c_sm=0;
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      sm_ints.set(index,iteration);
      int value;
      sm_ints.get(index,value);
      c_sm+=value; // making sure compiler optimization does not remove the code
      index++;
      if(index>=NB_ITEMS)
	index=0;
    }
  double f_sm = FrequencyMeasure::end();

  // report

  report(f_std,c_std,f_sm,c_sm);

  
  shared_memory::array<int>::clear(SEGMENT);

  
  // testing array of array of ints
  // ------------------------------
  
  // std
  
  std::array<std::array<int,10>,NB_ITEMS> std_a_ints;
  FrequencyMeasure::start();
  index=0;
  c_std=0;
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      std::array<int,10> a;
      a.fill(iteration);
      std_a_ints[index]=a;
      c_std += std_ints[index][0];
      index++;
      if(index>=NB_ITEMS)
	index=0;
    }
  f_std = FrequencyMeasure::end();

  // shared_memory
  
  shared_memory::array<int,10> sm_a_ints(SEGMENT,NB_ITEMS);
  FrequencyMeasure::start();
  index=0;
  c_sm=0;
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      int a[10];
      for(int i=0;i<10;i++) a[i]=iteration;
      sm_a_ints.set(index,a);
      int value[10];
      c_sm+=sm_a_ints.get(index,value)[0];
      index++;
      if(index>=NB_ITEMS)
	index=0;
    }
  f_sm = FrequencyMeasure::end();

  // report

  report(f_std,c_std,f_sm,c_sm);

  
  shared_memory::array<int>::clear(SEGMENT);
  
  
  // testing serializable item
  // -------------------------
  
  // std
  
  // similar to array of array
  
  // shared_memory
  
  shared_memory::array<shared_memory::Item<10>> sm_items(SEGMENT,NB_ITEMS);
  FrequencyMeasure::start();
  index=0;
  int c_sm=0;
  shared_memory::Item<10> g;
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      shared_memory::Item<10> s(iteration);
      sm_items.set(index,s);
      sm_items.get(index,g);
      c_sm+=g.get(0);
      index++;
      if(index>=NB_ITEMS)
	index=0;
    }
  double f_sm = FrequencyMeasure::end();

  // report

  report(f_std,c_std,f_sm,c_sm);
  
}

int main()
{

  std::cout << "\n\n--- 10 elements --\n";
  run<10>();

  std::cout << "\n\n--- 1 000 elements --\n";
  run<1000>();

  std::cout << "\n\n--- 100 000 elements --\n";
  run<100000>();
 
}
