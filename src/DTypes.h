#ifndef _DTYPES_H
#define _DTYPES_H
  #include "errors.h"
  #include "hashList.h"
  #include "list/LRU.h"
  #include "list/list.h"
  #include "MutexCondPair.h"

  typedef struct {
    LRU *consumerLRU;
    HashList *consumerMap;
    unsigned int maxCapacity;
    unsigned int consumerCount;
    MutexCondPair **mutexCondList; 
    void (*workGenerate)(void *);
  } Producer;

  typedef struct {
    void *data; // consumer can't free data assigned to them
    MutexCondPair *mc;
    void * (*callBack)(void *);
  } Consumer;

  Consumer *allocConsumer(void);
  Consumer *createConsumer(void);
  Consumer *initConsumer(Consumer *);
  Consumer *freeConsumer(Consumer *);

  void castFreeConsumer(void *);

  Producer *allocProducer();
  Producer *initProducer(Producer *prod, const unsigned int capacity);
  Producer *destroyProducer(Producer *prod);

  int insertConsumer(Producer *prod, Consumer *cons);
  int insertJob(Producer *prod, void *job, const int jobId);

#endif
