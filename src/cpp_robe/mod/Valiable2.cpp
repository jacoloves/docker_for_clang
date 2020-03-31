#include <iostream>
using namespace std;

int main() {
  int a = 0;

  // plus
  a += 2;
  cout << "a += 2          -> a = "<< a << endl;

  // incliment
  ++a;
  cout << "a++             -> a = "<< a << endl;

  // difficult formula
  a = a * 5 + 2;
  cout << "a = a * 5 + 2   -> a = "<< a << endl;

}
