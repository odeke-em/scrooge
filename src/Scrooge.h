// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _SCROOGE_H
#define _SCROOGE_H
  #include "DTypes.h"
  void *consume(void *pack);
  void *produce(void *pack);
  int insertJob(Producer *prod, void *job, const int jobId);

  HashList *map(List *dataSet, void *(*func)(void *));
#endif
