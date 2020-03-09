#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void siplay3DArray(int (*arr)[2][4], int rows) {
  for (int i=0; i<rows; i++) {
    for(int j=0; j<2; j++) {
      printf("{");
      for(int k=0; k<4; k++) {
        printf("%d ", arr[i][j][k]);
      }
      printf("}");
    }
    printf("\n");
  }
}
int main()
{
  int arr3d[3][2][4] = {
    {{1, 2, 3, 4}, {5, 6, 7, 8}},
    {{9, 10, 11, 12}, {13, 14, 15, 16}},
    {{17, 18, 19, 20}, {21, 22, 23, 24}}
  };
  siplay3DArray(arr3d, 3);
  return 0;
}