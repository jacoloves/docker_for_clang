#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  int v[5] = {1, 2, 3, 4, 5};
  int *pv = v;
  printf("%ld\n", sizeof(v)/sizeof(int));

  printf("%p\n", v);
  printf("%p\n", &v[0]);

  int matrix[2][3] = {{1, 2, 3}, {4, 5, 6}};

  for (int i = 0; i < 2; i++) {
    printf("&matrix[%d]: %p sizeof(matrix[%d]): %ld\n", i, &matrix[i], i, sizeof(matrix[i]));
  }

  return 0;
}