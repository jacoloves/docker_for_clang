#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void replace(char buffer[], char replacement, size_t size) {
  size_t count = 0;
  while(*buffer != NULL && count++<size) {
    *buffer = replacement;
    buffer++;
  }
}


int main()
{
  char name[8];
  strcpy(name, "Alexander");
  replace(name, '+', sizeof(name));
  printf("%s\n", name);
  return 0;
}