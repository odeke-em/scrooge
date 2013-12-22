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

MutexCondPair *freeMutexCond(MutexCondPair *mcPair) {
  if (mcPair != NULL) {
    execIfNotNull(mcPair->mutex, pthread_mutex_destroy);
    execIfNotNull(mcPair->condVar, pthread_cond_destroy);
    free(mcPair);
  }

  return mcPair;
}
#ifdef SAMPLE_MUTEX_COND_PAIR
int main() {
  return 0;
}
#endif
