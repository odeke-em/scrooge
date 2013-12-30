// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _SCROOGE_H
#define _SCROOGE_H
  #include "HLRU.h"
  #include "list/LRU.h"
  #include "MutexCondPair.h"

  typedef struct {
    HashList *consumerMap;
    LRU *consumerLRU;
    unsigned int maxCapacity;
    unsigned int consumerCount;
    MutexCondPair **mutexCondList; 
    void (*workGenerate)(void *);
  } Producer;

  Producer *allocProducer();
  Producer *initProducer(Producer *prod, const unsigned int capacity);

  int insertJob(Producer *prod, void *job, const int jobId);
#endif
