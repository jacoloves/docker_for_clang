#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  int (*(arr1[])) = {
    (int[]){0, 1, 2},
    (int[]){3, 4, 5},
    (int[]){6, 7, 8}
  };

  for(int j=0; j<3; j++) {
    for(int i=0; i<3; i++) {
      printf("arr1[%d][%d] Address: %p Value: %d\n", j, i, &arr1[j][i], arr1[j][i]);
    }
    printf("\n");
  }
  return 0;
}