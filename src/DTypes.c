#include <stdlib.h>
#include <stdio.h>

#include "DTypes.h"

int insertConsumer(Producer *prod, Consumer *cons) {
  if (prod != NULL && cons != NULL) {
    if (prod->consumerCount >= prod->maxCapacity) { // Time to purge
      prod->consumerLRU = purgeLRU(prod->consumerLRU);

      if (getListSize(prod->consumerLRU) == prod->consumerCount) {
	Consumer *popdConsumer =\
	  (Consumer *)listPop(&(prod->consumerLRU->head));
      }
    }
    return -1;
  } else {
    return 0;
  }
}

Consumer *allocConsumer(void) {
  return (Consumer *)malloc(sizeof(Consumer));
}

Consumer *initConsumer(Consumer *c) {
  if (c == NULL) {
    c = allocConsumer();
  }

  c->mc = NULL;
  c->data = NULL;
  c->callBack = NULL;

  return c;
}

Consumer *createConsumer(void) {
  Consumer *c = NULL;
  c = initConsumer(c);

  return c;
}

Consumer *freeConsumer(Consumer *c) {
  if (c != NULL) {
    MutexCondPair **mcByAddr = &(c->mc);
    if (mcByAddr != NULL) {
      *mcByAddr = freeMutexCondPair(*mcByAddr);
      *mcByAddr = NULL;
    }
  }

  return c;
}

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
