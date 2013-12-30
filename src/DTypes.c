#include <stdlib.h>
#include <stdio.h>

#include "DTypes.h"
#define DEBUG

int insertConsumer(Producer *prod, Consumer *newCons) {
  if (prod != NULL && newCons != NULL) {
    if (getListSize(prod->consumerLRU) >= prod->maxCapacity) { // Time to purge
    #ifdef DEBUG
      printf("\033[32mPurge in progess\033[00m\n");
    #endif
      prod->consumerLRU = purgeLRU(prod->consumerLRU);
      if (getListSize(prod->consumerLRU) >= prod->maxCapacity) {
	void *popdConsumer = popHead(prod->consumerLRU);
      #ifdef DEBUG 
	fprintf(stderr, "\033[33mFreeing consumer: %p\033[00m\n", popdConsumer);
      #endif
	castFreeConsumer(popdConsumer);
      } 
      prod->consumerLRU = setTagValue(prod->consumerLRU, 0);
    }

    setConsumerId(&newCons, getListSize(prod->consumerLRU));
    prod->consumerLRU = appendWithFreer(
      prod->consumerLRU, newCons, castFreeConsumer
    );

    return 0;
  } else {
    return -1;
  }
}

Consumer *allocConsumer(void) {
  return (Consumer *)malloc(sizeof(Consumer));
}

int setConsumerId(Consumer **c, const unsigned int id) {
  if (c != NULL && *c != NULL) {
    (*c)->id = id;
    return 0;
  } else {
    return -1;
  }
}

Consumer *initConsumer(Consumer *c) {
  if (c == NULL) {
    c = allocConsumer();
  }

  c->id = 0;
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

void castFreeConsumer(void *c) {
  if (c != NULL) {
    Consumer *castConsumer = (Consumer *)c;
    castConsumer = freeConsumer(castConsumer);
  }
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

Consumer *findConsumer(Producer *prod, const unsigned int id) {
  Consumer *consumerQuery = NULL;
  if (prod != NULL) {
    Consumer phonyConsumer;
    phonyConsumer.id = id;
    consumerQuery = (Consumer *)lookUpEntry(
      prod->consumerLRU, &phonyConsumer, consumerComp
    );
  }

  return consumerQuery;
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

Comparison consumerComp(const void *cA, const void *cB) {
  if (cA == NULL) {
    return cB == NULL ? Equal : Greater;
  } else {
    const Consumer *ccA = (Consumer *)cA;
    const Consumer *ccB = (Consumer *)cB;

    if (ccA->id != ccB->id) {
      return ccA->id < ccB->id ? Less : Greater;
    } else {
      return Equal;
    }
  }
}
