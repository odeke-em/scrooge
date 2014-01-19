// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _SCROOGE_H
#define _SCROOGE_H
  #include "DTypes.h"
  void *consume(void *pack);
  void *produce(void *pack);
  int insertJob(Producer *prod, void *job, const int jobId);

  HashList *map(List *dataSet, void *(*func)(void *));

  HashList *pMap(
    HashList *dataSet, void *(*func)(void *), const unsigned int thCount
  );

  // Combine the content of each hashList into a single List
  List *hCombine(HashList *dataSet);
#endif
