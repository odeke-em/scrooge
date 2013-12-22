// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "Scrooge.h"
#include "MutexCondPair.h"

#define PRODUCER_CAPACITY 40
#define DEFAULT_PRODUCER_CAPACITY 10

#define DEBUG

Producer *allocProducer() {
  return (Producer *)malloc(sizeof(Producer));
}

Producer *initProducer(Producer *prod, const unsigned int capacity) {
  if (prod == NULL) {
    prod = allocProducer();
  }

  if (prod == NULL) raiseError("Run out of memory");

  prod->consumerCount = 0;
  prod->workGenerate = NULL;
  prod->maxCapacity = capacity;

  prod->consumerList = NULL;
  prod->consumerList = initHashListWithSize(prod->consumerList, capacity);
  prod->mutexCondList = NULL; 
  if (capacity) {
    prod->mutexCondList = \
      (MutexCondPair **)malloc(sizeof(MutexCondPair *) * capacity);
  } else {
  }

  return prod;
}

Producer *destroyProducer(Producer *prod) {
  if (prod != NULL) {
    if (prod->consumerList != NULL) {
      destroyHLRU(prod->consumerList);
      prod->consumerList = NULL;
    }

    if (prod->mutexCondList != NULL) {
      int mIndex;
      for (mIndex = 0; mIndex < prod->maxCapacity; ++mIndex) {
	prod->mutexCondList[mIndex] = freeMutexCond(prod->mutexCondList[mIndex]);
      }

      free(prod->mutexCondList);
    }

    free(prod);
    prod = NULL;
  }

  return prod;
}

void *generateData(void) {
  return NULL;
}

int insertJob(Producer *prod, void *job, const int jobId) {
#ifdef DEBUG
  printf("\033[33mIn %s\033[00m\n", __func__);
#endif
  insertElem(prod->consumerList, job, jobId);

  return 0;
}

int main() {
  Producer *prod = NULL;
  prod = initProducer(prod, PRODUCER_CAPACITY);
#ifdef DEBUG
  printf("Scrooge!!\n");
#endif

  int i;
  for (i=0; i < 100; ++i) {
    int *intPtr = (int *)malloc(sizeof(int));
  #ifdef DEBUG
    printf("\033[94mInserting jobId: %d\n", i);
  #endif
    insertJob(prod, intPtr, i);
  }

  prod = destroyProducer(prod); 
  return 0;
}
