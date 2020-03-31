#include <iostream>
using namespace std;

int WesternToShouwa(int western) {
  if (1926 <= western && western <= 1989) {
    return western - 1925;
  } else {
    return 0;
  }
}

void Shouwa() {
  int western;

  cout << "西暦を教えて下さい > " << flush;
  cin >> western;

  int shouwa = WesternToShouwa(western);
  if (shouwa == 0) {
    cout << "その年は昭和ではありません。" << endl;
  } else {
    cout << "その年は昭和 " << shouwa << " 年です。" << endl;
  }
}

int main() {
  Shouwa();
  Shouwa();
}
