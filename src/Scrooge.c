// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "Scrooge.h"
#include "hashList.h"
#include "list/LRU.h"
#include "list/list.h"
#include "MutexCondPair.h"

#define PRODUCER_CAPACITY 5
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

  prod->consumerLRU = NULL;

  prod->consumerMap = NULL;
  prod->consumerMap = initHashListWithSize(prod->consumerMap, capacity);
  prod->mutexCondList = NULL; 
  if (capacity) {
    prod->mutexCondList = \
      (MutexCondPair **)malloc(sizeof(MutexCondPair *) * capacity);

    MutexCondPair **mcIt   = prod->mutexCondList,
		  **mcEnd  = mcIt + capacity;

    while (mcIt != mcEnd) {
      *mcIt = NULL;
      ++mcIt;
    }
  } else {
    ;
  }

  return prod;
}

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

Producer *destroyProducer(Producer *prod) {
  if (prod != NULL) {
    if (prod->consumerMap != NULL) {
      destroyHashList(prod->consumerMap);
      prod->consumerMap = NULL;
    }

    if (prod->consumerLRU != NULL) {
      destroyList(prod->consumerLRU);
      prod->consumerLRU = NULL;
    }

    if (prod->mutexCondList != NULL) {
      int mIndex;
      for (mIndex = 0; mIndex < prod->maxCapacity; ++mIndex) {
	prod->mutexCondList[mIndex] = \
	  freeMutexCondPair(prod->mutexCondList[mIndex]);
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
