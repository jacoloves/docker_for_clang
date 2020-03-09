#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int add(int num1, int num2){
  return num1 + num2;
}

int sub(int num1, int num2) {
  return num1 - num2;
}

typedef int (*fptrOperation)(int, int);

int compute(fptrOperation operation, int num1, int num2) {
  return operation(num1, num2);
}

int main()
{
  printf("%d\n", compute(add, 5, 6));
  printf("%d\n", compute(sub, 5, 6));

  return 0;
}