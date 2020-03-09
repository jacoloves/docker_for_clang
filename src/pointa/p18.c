#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    int *pv = (int*)malloc(5 * sizeof(int));
    for (int i = 0; i<5; i++) {
      //pv[i] = i + 1;
      *(pv+i) = i+1;
    }

    for (int i=0; i<5; i++) {
      printf("%d\n", *(pv+i));
    }

  return 0;
}