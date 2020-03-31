#include <iostream>
using namespace std;

void Divide() {
  int a, b;

  cout << "1 番目の値絵お入力してください > " << flush;
  cin >> a;
  
  cout << "2 番目の値絵お入力してください > " << flush;
  cin >> b;

  if (b == 0) {
    cout << "0 では割れません！" << endl;
  } else {
    cout << a << " / " << b << " = "
	 << a / b << " ... " << a % b << endl;
  }
}

int main() {
  Divide();
  Divide();
}
