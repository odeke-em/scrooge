// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "MutexCondPair.h"

#define execIfNotNull(var, func) {\
  if (var != NULL) func(var);\
}

MutexCondPair *allocMutexCondPair(void) {
  return (MutexCondPair *)malloc(sizeof(MutexCondPair));
}

MutexCondPair *initMutexCondPair(MutexCondPair *mcPair) {
  if (mcPair == NULL) {
    mcPair = allocMutexCondPair();
  }

  mcPair->mutex = NULL;
  mcPair->condVar = NULL;

  return mcPair;
}

MutexCondPair *freeMutexCondPair(MutexCondPair *mcPair) {
  if (mcPair != NULL) {
    execIfNotNull(mcPair->mutex, pthread_mutex_destroy);
    execIfNotNull(mcPair->condVar, pthread_cond_destroy);
    free(mcPair);
  }

  return mcPair;
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


#ifdef SAMPLE_MUTEX_COND_PAIR
int main() {
  return 0;
}
#endif
