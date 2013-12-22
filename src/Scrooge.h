#ifndef _SCROOGE_H
#define _SCROOGE_H
  #include "HLRU.h"
  #include "MutexCondPair.h"

  typedef struct {
    LRU *consumerList;
    unsigned int maxCapacity;
    unsigned int consumerCount;
    MutexCondPair **mutexCondList; 
    void (*workGenerate)(void *);
  } Producer;

  Producer *allocProducer();
  Producer *initProducer(Producer *prod, const unsigned int capacity);

  int insertJob(Producer *prod, void *job, const int jobId);
#endif
