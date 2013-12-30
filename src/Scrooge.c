// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "DTypes.h"
#include "Scrooge.h"
#include "hashList.h"
#include "list/LRU.h"
#include "MutexCondPair.h"

#define PRODUCER_CAPACITY 5
#define DEFAULT_PRODUCER_CAPACITY 10

#define DEBUG

void *consume(void *pack) {
  LRU *results = NULL;
  if (pack != NULL) {
    Consumer *cons = (Consumer *)pack;
    MutexCondPair *mc = cons->mc;
    while (1) {
      fprintf(stderr, "Consumer awaiting signal for worker\n");
      int status = pthread_cond_wait(mc->condVar, mc->mutex);
      fprintf(stderr, "Consumer received signal to start work\n");
      pthread_mutex_lock(mc->mutex);

      // Consume the resources and return the results
      results = prepend(results, cons->callBack(cons->data));
      pthread_cond_broadcast(mc->condVar);
      fprintf(stderr, "Consumer unlocked and ready to receive data\n");
      pthread_mutex_unlock(mc->mutex);
    }
  }

  return results;
}

void *generateData(void) {
  return NULL;
}

int insertJob(Producer *prod, void *job, const int jobId) {
#ifdef DEBUG
  printf("\033[33mIn %s\033[00m\n", __func__);
#endif
  insertElem(prod->consumerMap, job, jobId);

  return 0;
}

int main() {
  Producer *prod = NULL;
  prod = initProducer(prod, PRODUCER_CAPACITY);
#ifdef INSERT_JOB
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
#endif

  prod = destroyProducer(prod); 
  return 0;
}
