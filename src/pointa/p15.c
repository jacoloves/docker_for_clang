#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  int v[5] = {1, 2, 3, 4, 5};
  printf("%ld\n", sizeof(v)/sizeof(int));

  return 0;
}