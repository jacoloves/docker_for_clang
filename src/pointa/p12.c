#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int (*operation)(int, int);
operation operations[128] = {NULL};

int add(int num1, int num2){
  return num1 + num2;
}

int sub(int num1, int num2) {
  return num1 - num2;
}


void initializeOperationsArray() {
  operations['+'] = add;
  operations['-'] = sub;
}

typedef int (*fptrOperation)(int, int);


int evaluateArray(char opcode, int num1, int num2) {
  fptrOperation operation;
  operation = operations[opcode];
  return operation(num1, num2);
}

int main()
{
  initializeOperationsArray();
  printf("%d\n", evaluateArray('+', 5, 6));
  printf("%d\n", evaluateArray('-', 5, 6));
  return 0;
}