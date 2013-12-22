#ifndef _MUTEX_COND_PAIR_H
#define _MUTEX_COND_PAIR_H
  #include <pthread.h>
  typedef struct {
    pthread_cond_t *condVar;
    pthread_mutex_t *mutex;
  } MutexCondPair;

  MutexCondPair *allocMutexCondPair(void);
  MutexCondPair *freeMutexCond(MutexCondPair *mcPair);
  MutexCondPair *initMutexCondPair(MutexCondPair *mcPair);
#endif
