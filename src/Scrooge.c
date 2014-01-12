// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset

#include "errors.h"
#include "Scrooge.h"
#include "hashList.h"
#include "list/LRU.h"
#include "MutexCondPair.h"

#define PRODUCER_CAPACITY 5
#define DEFAULT_PRODUCER_CAPACITY 10

HashList *map(List *dataSet, void *(*func)(void *)) {
  HashList *results = NULL;
  if (dataSet != NULL && func != NULL) {
    results = initHashListWithSize(results, getListSize(dataSet));
    Node *it = dataSet->head, *end = dataSet->tail; 
    unsigned int elemIndex = 0;
    while (it != NULL) {
      insertElem(results, func(it->data), elemIndex);
      it = getNextNode(it); 
      ++elemIndex;
    }
  }

  return results;
}

void *consume(void *pack) {
  // Returns results starting in reverse 
  // chronological order ie latest first
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

void *produce(void *pack) {
  if (pack != NULL) {
    // Incomplete
  }
  return NULL;
}

int insertJob(Producer *prod, void *job, const int jobId) {
#ifdef DEBUG
  printf("\033[33mIn %s\033[00m\n", __func__);
#endif
  insertElem(prod->consumerMap, job, jobId);

  return 0;
}

List *squareToTen(void *start) {
  List *resultL = NULL;
  if (start != NULL) {
    int iStart = *(int *)start, end = iStart + 10;
    while (iStart <= end) {
      int *sqValue = (int *)malloc(sizeof(int));
      *sqValue = (iStart *2);
      resultL = appendAndTag(resultL, sqValue, Heapd, free);
      ++iStart;
    }
  }

  return resultL;
}

List *merge(const char *fmt, ...) {
  ;
}

int main() {
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

#ifdef CONSUMER_PRODUCER_DEMO
  Producer *prod = NULL;
  prod = initProducer(prod, 5);

  int idx;
  for (idx=0; idx < 40;  ++idx) {
    Consumer *cn = createConsumer();
    printf("Insertion: %d\n", insertConsumer(prod, cn));
  }

  Consumer *consumerA = findConsumer(prod, 1);
  Consumer *consumerB = findConsumer(prod, getListSize(prod->consumerLRU));

 
  if (getListSize(prod->consumerLRU)) {
    assert(consumerA);
  } else {
    assert(consumerA == NULL);
  }
  
  assert(NULL == consumerB);

  printf("consumerA: %p\n", consumerA);
  printf("consumerB: %p\n", consumerB);

  prod = destroyProducer(prod); 
#endif

  List *l = NULL;
  int i;
  for (i=0; i < 90000; i += 11) {
    int *intPtr = (int *)malloc(sizeof(int));
    *intPtr = i;
  #ifdef DEBUG
    printf("\033[94mValue: %d\n", i);
  #endif
    l = appendAndTag(l, intPtr, Heapd, free);
  }

  HashList *sqrdH = map(l, (void *)squareToTen);

  Element **it = sqrdH->list, **end = it + sqrdH->size;
  List *l1 = it[0]->value;
  List *l2 = it[1]->value;
  List *l3 = it[2]->value;
  List *l4 = it[3]->value;
  List *l7000 = it[6999]->value;

  List *merged = multiMerge(5, l4, l1, l7000, l3, l2, intPtrComp);
  printf("After merging: \n");
  printList(merged);
  printf("\n");
  destroyList(merged);

  for (it=sqrdH->list; it < end; ++it) { 
    if (*it != NULL) {
    #ifdef SHOW_CONTENT
      printList((*it)->value);
      printf("\n");
    #endif
      destroyList((*it)->value);
      (*it)->value = NULL;
    }
  }

  destroyHashList(sqrdH);
  destroyList(l);
  return 0;
}
