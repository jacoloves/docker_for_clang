#include <iostream>
using namespace std;

size_t StrLen(char* str) {
  int i;
  for (i = 0; *(str + i) != '\0'; ++i) {
    // no process
  }
  return i;
}

void ShowLength(char* str) {
  cout << "String[" << str << "]'s length is "
       << StrLen(str) << " byte." << endl;
}

int main() {
  ShowLength("Hello");
  ShowLength("");
}
