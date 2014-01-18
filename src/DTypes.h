#ifndef _DTYPES_H
#define _DTYPES_H
  #include "errors.h"
  #include "hashList.h"
  #include "list/LRU.h"
  #include "list/list.h"
  #include "MutexCondPair.h"

  typedef struct {
    Element **src, **dest;
    unsigned int startIndex, endIndex;
    void *(*func)(void *);
  } DictSliceAndFunc;

  struct DSlicePair {
    struct DictSliceAndFunc *srcSlice;
    struct DictSliceAndFunc *destSlice;
  };

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
    unsigned int id; // Unique Identifier
    unsigned int ready:1; // Set once work is being done
    unsigned int sourceId;
    void * (*callBack)(void *);
  } Consumer;

  Consumer *allocConsumer(void);
  Consumer *createConsumer(void);
  Consumer *initConsumer(Consumer *);
  Consumer *freeConsumer(Consumer *);

  int setConsumerId(Consumer **c, const unsigned int id);
  void castFreeConsumer(void *);

  Producer *allocProducer();
  Producer *initProducer(Producer *prod, const unsigned int capacity);
  Producer *destroyProducer(Producer *prod);

  Comparison consumerComp(const void *, const void *);

  int insertConsumer(Producer *prod, Consumer *cons);
  int insertJob(Producer *prod, void *job, const int jobId);

  Consumer *findConsumer(Producer *prod, const unsigned int id);
#endif
