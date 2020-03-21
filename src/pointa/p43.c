#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct _factorialData {
  int number;
  int result;
  void (*callBack)(struct _factorialData);
} FactorialData;

void factorial(void *args) {
  FactorialData *factorialData = (FactorialData*) args;
  void (*callBack)(FactorialData*);

  int number = factorialData->number;
  callBack = factorialData->callBack;

  int num = 1;
  for (int i=1; i<=number; i++) {
    num *= i;
  }

  factorialData->result = num;
  callBack(factorialData);

  pthread_exit(NULL);
}

void startThread(FactorialData *data) {
  pthread_t thread_id;
  int thread = pthread_create(&thread_id, NULL, factorial, (void *)data);
}

void callBackFunction(FactorialData *factionalData) {
  printf("Factorial is %d\n", factionalData->result);
}

int main()
{
  FactorialData *data = (FactorialData*)malloc(sizeof(FactorialData));

  if(!data) {
    printf("Failed to allocate memory\n");
    return 0;
  }

  data->number = 5;
  data->callBack = callBackFunction;

  startThread(data);

  Sleep(2000);
  return 0;
}