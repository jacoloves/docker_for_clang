#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct _person {
  char* firstName;
  char* lastName;
  char* title;
  unsigned int age;
} Person;

typedef struct _alternatePerson {
  char* firstName;
  char* lastName;
  char* title;
  short age;
} AlternatePerson;

int main()
{
  Person person;
  person.firstName = (char*)malloc(strlen("Ellen")+1);
  strcpy(person.firstName, "Emily");
  person.age = 23;

  Person *ptrPerson;
  ptrPerson = (Person*)malloc(sizeof(Person));
  ptrPerson->firstName = (char*)malloc(strlen("Emilly")+1);
  strcpy(ptrPerson->firstName, "Emilly");
  ptrPerson->age = 23;

  printf("%d\n", ptrPerson->age);
  return 0;
}