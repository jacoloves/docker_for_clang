#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int (*fptrOperation)(int, int);

int add(int num1, int num2){
  return num1 + num2;
}

int sub(int num1, int num2) {
  return num1 - num2;
}

fptrOperation select(char opcode) {
  switch(opcode) {
    case '+': return add;
    case '-': return sub;
  }
}

int evaluate(char opcode, int num1, int num2) {
  fptrOperation operation = select(opcode);
  return operation(num1, num2);
}

int main()
{
  printf("%d\n", evaluate('+', 5, 6));
  printf("%d\n", evaluate('-', 5, 6));
  return 0;
}