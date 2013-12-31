// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "errors.h"
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

MutexCondPair *createMutexCondPair(void) {
  MutexCondPair *freshMC = NULL;
  freshMC = initMutexCondPair(freshMC);
  freshMC->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  freshMC->condVar = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));

  int errCode = 0;
  if ((errCode = pthread_cond_init(freshMC->condVar, NULL))) {
    raiseError(errCode);
  }

  if ((errCode = pthread_mutex_init(freshMC->mutex, NULL))) {
    raiseError(errCode);
  }
    

  return freshMC;
}

MutexCondPair *freeMutexCondPair(MutexCondPair *mcPair) {
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
