#include "shared_memory/array.hpp"

class Pitoune
{
public:

  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( a,b );
  }
  void print()
  {
    std::cout << "pitoune: " << a << " | " << b << "\n";
  }
  
  int a;
  double b;
  
};

int main()
{
  shared_memory::Array<Pitoune> a("memory",
				  10,
				  true,
				  true);

  
  Pitoune p0;
  p0.a = 0;
  p0.b = 0;

  Pitoune p1;
  p1.a = 1;
  p1.b = 1;

  
  a.set(0,p0);
  a.set(1,p1);

  a.get(0).print();
  a.get(1).print();
  
}
