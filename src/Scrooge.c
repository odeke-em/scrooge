// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include <pthread.h> // For memset

#include "errors.h"
#include "Scrooge.h"
#include "hashList.h"
#include "list/LRU.h"
#include "MutexCondPair.h"

#define PRODUCER_CAPACITY 5
#define DEFAULT_PRODUCER_CAPACITY 10

void *consumeElem(void *d) {
  void *result = NULL;
  if (d != NULL) {
    DictSliceAndFunc *dSlice = (DictSliceAndFunc *)d;
    Element **src = dSlice->src,
            **dest = dSlice->dest;
    unsigned int i;
    void *(*func)(void *) = dSlice->func;
    for (i=dSlice->startIndex; i < dSlice->endIndex; ++i) {
      dest[i] = addToHead(dest[i], func(src[i]->value));
    }
  }

  return result;
}

HashList *pMap(HashList *dataSet, void *(*func)(void *), unsigned int thCount) {
  HashList *mapped = NULL;;
  if (dataSet != NULL && func != NULL && thCount) {
    mapped = initHashListWithSize(mapped, dataSet->size);
    pthread_t threadL[thCount];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

  #ifdef __linux__
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  #endif

    unsigned int chunkSz = (thCount > 1 ? dataSet->size/thCount : dataSet->size);
    DictSliceAndFunc sliceArray[thCount];
    unsigned int startIndex, endIndex, endFound, thIndex;

    thIndex = endFound = 0;
    startIndex = endIndex = 0;

    while (! endFound) {
      startIndex  = endIndex;
      endIndex   += chunkSz + 1; // Ceiling

      if (endIndex  >= dataSet->size) {
        endIndex = dataSet->size;
        endFound = 1;
      }

      DictSliceAndFunc *dSlice = &sliceArray[thIndex];
      dSlice->src = dataSet->list;
      dSlice->dest = mapped->list;
      dSlice->func = func;
      dSlice->startIndex = startIndex;
      dSlice->endIndex = endIndex;
    #ifdef DEBUG
      printf("start: %d end: %d\n", startIndex, endIndex);
    #endif
      pthread_create(&threadL[thIndex], &attr, consumeElem, (void *)dSlice);
      ++thIndex;
    }

    for (thIndex=0; thIndex < thCount; ++thIndex) {
    #ifdef DEBUG
      printf("thIndex: %d\n", thIndex);
    #endif
      void *retSav = NULL;
      if (pthread_join(threadL[thIndex], &retSav)) {
        raiseError("Failed to join thread");
      }
    }
  }

  return mapped;
}

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
      printf("mc: %p\n", mc);
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
      *sqValue = (iStart * iStart);
      resultL = appendAndTag(resultL, sqValue, Heapd, free);
      ++iStart;
    }
  }

#ifdef DEBUG
  printf("\033[94m");
  printList(resultL);
  printf("\033[00m\n");
#endif

  return resultL;
}

List *merge(const char *fmt, ...) {
  ;
}

int main() {
#ifdef INSERT_JOB
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
  prod = initProducer(prod, 9);

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

// #define SINGLE_TH_MAP
  List *l = NULL;
  int i, maxValue = 999999;
  HashList *hl = NULL;
  hl = initHashListWithSize(hl, maxValue);
  for (i=0; i < maxValue; i += 1) {
  #ifdef DEBUG
    printf("\033[95mValue: %d\n", i);
  #endif
  #ifdef SINGLE_TH_MAP
    int *intPtr = (int *)malloc(sizeof(int));
    *intPtr = i;
    l = appendAndTag(l, intPtr, Heapd, free);
  #else
    int *hIntPtr = (int *)malloc(sizeof(int));
    *hIntPtr = i;
    insertElem(hl, hIntPtr, i);
  #endif
  }

  HashList *selectedH = NULL;
#ifdef SINGLE_TH_MAP
  printf("ogMap selected\n");
  selectedH = map(l, (void *)squareToTen);
#else
  printf("pMap selected\n");
  selectedH = pMap(hl, (void *)squareToTen, 17);
#endif

  printf("selectedH: %p\n", selectedH);
  Element **it = selectedH->list, **end = it + selectedH->size;
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

// #define SHOW_CONTENT
  for (it=selectedH->list; it < end; ++it) { 
    if (*it != NULL) {
    #ifdef SHOW_CONTENT
      printList((*it)->value);
      printf("\n");
    #endif
      destroyList((*it)->value);
      (*it)->value = NULL;
    }
  }

  printf("pM: %d\n", selectedH->size);
  destroyHashList(selectedH);
  destroyList(l);
  return 0;
}
