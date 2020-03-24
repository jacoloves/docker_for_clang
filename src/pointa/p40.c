#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  int num = 8;
  int *pi = (int*)num;

  pi = &num;
  printf("Before: %p\n", pi);
  int tmp = (int)pi;
  pi = (int*)tmp;
  printf("After: %p\n", pi);

  return 0;
}