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

#define LIST_SIZE 10
Person *list[LIST_SIZE];

void initializeList() {
    for(int i=0; i<LIST_SIZE; i++) {
        list[i] = NULL;
    }
}

void initializePerson(Person *person, const char* fn, const char* ln, const char* title, int age) {
  person->firstName = (char*)malloc(strlen(fn)+1);
  strcpy(person->firstName, fn);
  person->lastName = (char*)malloc(strlen(fn)+1);
  strcpy(person->lastName, ln);
  person->title = (char*)malloc(strlen(title)+1);
  strcpy(person->title, title);
  person->age = age;
}

void deallocatePerson(Person *person) {
  free(person->firstName);
  free(person->lastName);
  free(person->title);
}

void processPerson() {
  Person person;
  initializePerson(&person, "Peeter", "Underwood", "Manager", 36);

  Person *ptrPerson;
  ptrPerson = (Person*)malloc(sizeof(Person));

  initializePerson(ptrPerson, "chiko", "Gabriel", "Manager", 36);

  for (int i = 0; i<strlen(ptrPerson->lastName); i++) {
    printf("%c", ptrPerson->lastName[i]);
  }
  printf("\n");
  deallocatePerson(ptrPerson);
}

Person *getPerson() {
    for(int i=0; i<LIST_SIZE; i++) {
        if(list[i] != NULL) {
            Person *ptr = list[i];
            return ptr;
        }
    }
    Person *person = (Person*)malloc(sizeof(Person));
    return person;
    
}

Person *returnPerson(Person *person) {
    for(int i=0; i<LIST_SIZE; i++) {
        if(list[i] == NULL) {
            list[i] = person;
            return person;
        }
    }
    deallocatePerson(person);
    free(person);
    return NULL;
}

int main()
{
  initializeList();
  Person *ptrPerson;

  ptrPerson = getPerson();
  initializePerson(ptrPerson, "Ralph", "Fitsgerald", "Mr.", 35);
  ptrPerson = returnPerson(ptrPerson);

  return 0;
}