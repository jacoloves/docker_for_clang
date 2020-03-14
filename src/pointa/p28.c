#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  char *tabHeader = "Sound";
  *tabHeader = 'L';
  printf("%s\n", tabHeader);
  return 0;
}