#include <iostream>
using namespace std;

int main() {
  int a;

  cout << "Input value > " << flush;
  cin >> a;

  cout << "その数を3で割ったあまりは " << a % 3 << " です。" << endl;
}
