// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _MUTEX_COND_PAIR_H
#define _MUTEX_COND_PAIR_H
  #include <pthread.h>
  
  typedef struct MutexCondPair_ {
    pthread_cond_t *condVar;
    pthread_mutex_t *mutex;
  } MutexCondPair;

  MutexCondPair *allocMutexCondPair(void);
  MutexCondPair *createMutexCondPair(void);
  MutexCondPair *initMutexCondPair(MutexCondPair *mcPair);
  MutexCondPair *freeMutexCondPair(MutexCondPair *mcPair);
#endif
