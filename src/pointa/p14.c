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

typedef int (*fptrToSingleInt)(int);
typedef int (*fptrToTwoInts)(int, int);
int add(int, int);

typedef void (*fptrBase)();

int main()
{
  fptrBase basePointer;
  fptrToTwoInts fptrFirst = add;
  basePointer = (fptrToSingleInt)fptrFirst;
  fptrFirst = (fptrToTwoInts)basePointer;
  printf("%d\n", fptrFirst(5, 6));

  return 0;
}