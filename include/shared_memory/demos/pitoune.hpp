class Pitoune
{
public:

  Pitoune(){}
  
  Pitoune(int value)
    : a(value),
      b(static_cast<double>(value)){}
  
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
