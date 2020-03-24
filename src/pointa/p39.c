#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct _item {
  int partNumber;
  int quantity;
  int binNumber;
} Item;

int main()
{
  Item part = {1234, 35, 107};
  //int *pi = &part.partNumber;
  printf("Part number: %d\n", part.partNumber);
  //pi++;
  //pi = &part.quantity;
  printf("Quantity: %d\n", part.quantity);
  //pi++;
  //pi = &part.binNumber;
  printf("Bin number: %d\n", part.binNumber);

  return 0;
}