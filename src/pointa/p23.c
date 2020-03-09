#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void display2DArray(int arr[][5], int rows) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < 5; j++) {
      printf("%d", arr[i][j]);
    }
    printf("\n");
  }
}
int main()
{
  int matrix[2][5] = {
    {1, 2, 3, 4, 5}, 
    {6, 7, 8, 9, 10}
  };

  display2DArray(matrix, 2);
  return 0;
}