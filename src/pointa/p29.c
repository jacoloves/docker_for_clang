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

void initializePerson(Person *person, const char* fn, const char* ln, const char* title, int age) {
  person->firstName = (char*)malloc(strlen(fn)+1);
  strcpy(person->firstName, fn);
  person->lastName = (char*)malloc(strlen(fn)+1);
  strcpy(person->lastName, ln);
  person->title = (char*)malloc(strlen(title)+1);
  strcpy(person->title, title);
  person->age = age;
}

void processPerson() {
  Person person;
  initializePerson(&person, "Peeter", "Underwood", "Manager", 36);

  for (int i = 0; i<strlen(person.firstName); i++) {
    printf("%c\n", person.firstName[i]);
  }
}

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

  AlternatePerson otherPerson;

  printf("%d\n", sizeof(Person));

  printf("%d\n", ptrPerson->age);
  return 0;
}