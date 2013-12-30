// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _MUTEX_COND_PAIR_H
#define _MUTEX_COND_PAIR_H
  #include <pthread.h>
  
  typedef struct MutexCondPair_ {
    pthread_cond_t *condVar;
    pthread_mutex_t *mutex;
  } MutexCondPair;

  typedef struct {
    void *data; // consumer can't free data assigned to them
    MutexCondPair *mc;
    void * (*callBack)(void *);
  } Consumer;

  MutexCondPair *allocMutexCondPair(void);
  MutexCondPair *createMutexCondPair(void);
  MutexCondPair *initMutexCondPair(MutexCondPair *mcPair);
  MutexCondPair *freeMutexCondPair(MutexCondPair *mcPair);

  Consumer *allocConsumer(void);
  Consumer *createConsumer(void);
  Consumer *initConsumer(Consumer *);
  Consumer *freeConsumer(Consumer *);
#endif
