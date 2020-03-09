#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void display2DArrayUnknownSize(int *arr, int rows, int cols) {
  for(int i=0; i<rows; i++) {
    for(int j=0; j<cols; j++) {
      printf("%d ", *(arr + (i*cols) + j));
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

  display2DArrayUnknownSize(&matrix[0][0], 2, 5);

  return 0;
}