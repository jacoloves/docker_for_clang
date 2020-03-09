#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char* trim(char* pharse) {
  char* old = pharse;
  char* new = pharse;

  while(*old == ' ') {
    old++;
  }

  while(*old) {
    *(new++) = *(old++);
  }
  *new = 0;
  return (char*) realloc(pharse,strlen(pharse)+1);
}

int main()
{
  char* buffer = (char*)malloc(strlen(" cat")+1);
  strcpy(buffer," cat");
  printf("%s\n", trim(buffer));

  return 0;
}