#include <stdio.h>
#include "HLRU.h"

#define DEFAULT_CACHE_SIZE 200

void destroyHLRU(LRU *cache) {
  destroyHashList(cache);
}

#ifdef SAMPLE_HLRU
int main() {
  LRU *cache = NULL;
  cache = initHashListWithSize(cache, DEFAULT_CACHE_SIZE);
  destroyHashList(cache);
  return 0;
}
#endif
