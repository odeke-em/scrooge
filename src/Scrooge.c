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
    Consumer *c = (Consumer *)d;
    if (c->callBack != NULL && c->data != NULL) {
      Element **e = (Element **)c->data;
      List *resList = NULL;
      resList = c->callBack((*e)->value);
      result = (void *)resList;
    }

    c->ready= 1;
  }
  return result;
}

int readyCheck(void *data) {
  return data == NULL ? 0 : (((Consumer *)data)->ready == 1);
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

    LRU *activeConsumers = NULL, *idleConsumers = NULL;
    int i;
    for (i=0; i < thCount && i < dataSet->size; ++i) {
      Consumer *c = createConsumer();
      c->id = c->sourceId = i;
      c->data = get(dataSet, i);
      c->callBack = func;
      c->ready = 0;
     
      pthread_create(&threadL[c->sourceId], &attr, consumeElem, (void *)c); 
      activeConsumers = appendAndTag(
        activeConsumers, c, Heapd, (void *)freeConsumer
      );
    }

    while (i < dataSet->size) {
      // First phase find all Consumers that are done with their tasks
      activeConsumers = purgeAndSaveByQuantify(
        activeConsumers, &idleConsumers, readyCheck
      );
      // Pop as many from the idleConsumers list
      while (! isEmpty(idleConsumers)) {
	Consumer *readyCon = (Consumer *)popHead(idleConsumers);
	if (readyCon != NULL) {
	  void *resultSav = NULL;

	  if (pthread_join(threadL[readyCon->sourceId], &resultSav)) {
	    raiseError("Failed to join thread");
	  }
	  insertElem(mapped, resultSav, readyCon->id);

	  // Fresh now for re-deployment
          // No need to reset the threadId
	  readyCon->data = get(dataSet, i);
	  readyCon->ready = 0;
	  readyCon->id = i;
	  activeConsumers = appendAndTag(
            activeConsumers, readyCon, Heapd, (void *)freeConsumer
          );

          pthread_create(
            &threadL[readyCon->sourceId], &attr, consumeElem, (void *)readyCon
          ); 
          ++i;
	}
      }
    }

    while (! isEmpty(activeConsumers)) {
      Consumer *activCon = (Consumer *)popHead(activeConsumers);
      if (activCon != NULL) {
	void *resultSav = NULL;
	if (pthread_join(threadL[activCon->id], &resultSav)) {
	  raiseError("Failed to join thread");
	}
	insertElem(mapped, resultSav, activCon->id);
      }

      freeConsumer(activCon);
    }

    destroyList(idleConsumers);
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
  int i, maxValue = 90999;
  HashList *hl = NULL;
  hl = initHashListWithSize(hl, maxValue);
  for (i=0; i < maxValue; i += 1) {
  #ifdef DEBUG
    printf("\033[94mValue: %d\n", i);
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
  selectedH = pMap(hl, (void *)squareToTen, 10);
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
