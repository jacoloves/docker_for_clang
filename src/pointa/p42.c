#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  double *vectorA;
  double *vectorB;
  double sum;
  int length;
} VectorInfo;

typedef struct {
  VectorInfo *info;
  int beginningIndex;
} Product;

pthread_attr_t mutexSum;

void dotProduct(void *prod) {
  Product *product = (Product*)prod;
  VectorInfo *vectorInfo = product->info;
  int beginningIndex = product->beginningIndex;
  int endingIndex = beginningIndex * vectorInfo->length;
  double total = 0;

  for (int i = beginningIndex; i < endingIndex; i++) {
    total += (vectorInfo->vectorA[i] * vectorInfo->vectorB[i]);
  }

  pthread_mutex_lock(&mutexSum);
  vectorInfo->sum += total;
  pthread_mutex_unlock(&mutexSum);

  pthread_exit((void*) 0);
}

#define NUM_THREADS 4

void threadExample() {
  VectorInfo vectorInfo;
  double vectorA[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,
                      9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
  double vectorB[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,
                      9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};

  double sum;

  vectorInfo.vectorA = vectorA;
  vectorInfo.vectorB = vectorB;

  vectorInfo.length = 4;

  pthread_t threads{NUM_THREADS};

  void *status;
  pthread_attr_t attr;

  pthread_mutex_init(&mutexSum, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


}

int main()
{

  return 0;
}