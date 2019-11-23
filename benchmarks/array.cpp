#include "shared_memory/array.hpp"
#include "shared_memory/demos/item.hpp"
#include <chrono>


#define SEGMENT "benchmark_array"
#define NB_ITERATIONS 250000


// what this does:
// compute the difference for writing/reading items
// to/from std::array and shared_memory::array

template<typename T>
class Add
{
  
public:

  Add():v_(0){}
  
  template<typename Q=T>
  typename std::enable_if<std::is_fundamental<Q>::value, void>::type
  add(const T& t)
  {
    v_+=static_cast<int>(t);
  }

  template<typename Q=T>
  typename std::enable_if<!std::is_fundamental<Q>::value, void>::type
  add(const T& t)
  {
    v_+=t.get();
  }

  int v_;
  
};



template< typename T, int NB_ITEMS >
void run()
{

  Add<T> add_std;
  Add<T> add_sm;
  
  std::array<T,NB_ITEMS> std_a;

  T item1(1);
  T item2;
  
  auto start = std::chrono::steady_clock::now();


  int index=6;
  
  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      std_a[index]=item1;
      item2 = std_a[index];
      add_std.add(item2); // making sure compiler optimization does not remove the code
    }

  auto end = std::chrono::steady_clock::now();
  long int duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  double duration_seconds = static_cast<double>(duration_us)/1e6;
  double frequency_std = static_cast<double>(NB_ITERATIONS)/duration_seconds;

  std::cout << "frequency std: " << frequency_std << " | " << add_std.v_ << "\n";  

  shared_memory::array<T> sm_a(SEGMENT,
			       NB_ITEMS,
			       true,
			       false);


  start = std::chrono::steady_clock::now();

  for(uint iteration=0;
      iteration<NB_ITERATIONS;
      iteration++)
    {
      sm_a.set(index,item1);
      item2 = sm_a.get(index);
      add_sm.add(item2);
    }

  end = std::chrono::steady_clock::now();
  duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  duration_seconds = static_cast<double>(duration_us)/1e6;
  double frequency_sm = static_cast<double>(NB_ITERATIONS)/duration_seconds;

  std::cout << "frequency sm: " << frequency_sm << " | " << add_sm.v_ << "\n";  

  std::cout << "ratio: " << frequency_std/frequency_sm << "\n";
  
}

int main()
{

  shared_memory::array<int>::clear(SEGMENT);

  std::cout << "\n\n100 instances of size 10\n";
  run<shared_memory::Item<10>,100>();

  std::cout << "\n\n1 000 instances of size 10\n";
  run<shared_memory::Item<10>,1000>();

  std::cout << "\n\n1 000 instances of size 100\n";
  run<shared_memory::Item<100>,1000>();

  std::cout << "\n\n1 0000 instances of size 100\n";
  run<shared_memory::Item<100>,10000>();

  std::cout << "\n\n1 000 ints\n";
  run<int,1000>();

  std::cout << "\n\n100 000 ints\n";
  run<int,100000>();

  std::cout << "\n\n1 000 000 ints\n";
  run<int,1000000>();

  
  
  std::cout << "\n\n";
  
  
}
